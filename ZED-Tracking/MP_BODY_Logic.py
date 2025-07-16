"""Tracciamento dell'orientazione dei polsi tramite ZED e MediaPipe.

Questo script acquisisce le immagini da una telecamera ZED, rileva la
posizione del corpo con MediaPipe Pose e calcola l'orientamento dei polsi.
Le informazioni ottenute vengono inviate tramite UDP.
"""

import argparse
import json
import os
import socket
import cv2
import mediapipe as mp
import numpy as np
import pyzed.sl as sl

# --- Configurazione UDP -----------------------------------------------------
def setup_udp(ip: str, port: int) -> socket.socket:
    """Crea e restituisce un socket UDP pronto per l'invio."""

    # Creiamo un semplice socket datagram che useremo per inviare i dati
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return sock


# --- Configurazione della ZED ------------------------------------------------
def setup_zed():
    """Inizializza la telecamera ZED e restituisce gli oggetti utili."""

    # Creiamo l'istanza della camera
    zed = sl.Camera()

    # Parametri d'inizializzazione: risoluzione e modalità di profondità
    init_params = sl.InitParameters()
    init_params.camera_resolution = sl.RESOLUTION.HD720
    init_params.depth_mode = sl.DEPTH_MODE.PERFORMANCE

    # Apertura della camera; in caso di errore solleviamo un'eccezione
    if zed.open(init_params) != sl.ERROR_CODE.SUCCESS:
        raise RuntimeError("ZED Open Error")

    # Matrice per l'immagine e per la cloud di punti 3D
    image_zed = sl.Mat()
    point_cloud = sl.Mat()

    # Parametri runtime (usati ad ogni grab)
    runtime_params = sl.RuntimeParameters()
    return zed, image_zed, point_cloud, runtime_params

# --- Configurazione di MediaPipe Pose ---------------------------------------
def setup_pose():
    """Restituisce l'istanza di Pose e gli strumenti di disegno."""

    mp_pose = mp.solutions.pose
    mp_drawing = mp.solutions.drawing_utils

    # Inizializziamo Pose con parametri di default adatti a uno stream video
    pose = mp_pose.Pose(
        static_image_mode=False,
        model_complexity=1,
        enable_segmentation=False,
        min_detection_confidence=0.5,
        min_tracking_confidence=0.5,
    )
    return mp_pose, mp_drawing, pose

def clamp_point(x: float, y: float, w: int, h: int) -> tuple[int, int]:
    """Limita le coordinate normalizzate di MediaPipe ai bordi del frame."""

    # MediaPipe fornisce valori normalizzati tra 0 e 1. Qui li convertiamo in
    # coordinate pixel assicurandoci che rientrino nell'immagine per evitare
    # errori quando sono fuori dal campo visivo.
    px = int(np.clip(x * w, 0, w - 1))
    py = int(np.clip(y * h, 0, h - 1))
    return px, py


def process_wrist(
    landmarks,
    wrist_id: int,
    pinky_id: int,
    index_id: int,
    label: str,
    w: int,
    h: int,
    point_cloud: sl.Mat,
    frame: np.ndarray,
    message: dict,
):
    # Estraiamo i landmark necessari
    wrist = landmarks[wrist_id]
    pinky = landmarks[pinky_id]
    index = landmarks[index_id]

    # Coordinate del polso nel frame e nel cloud di punti
    cx, cy = clamp_point(wrist.x, wrist.y, w, h)
    wrist_pt = np.array(point_cloud.get_value(cx, cy)[1])[:3]

    ix, iy = clamp_point(index.x, index.y, w, h)
    index_pt = np.array(point_cloud.get_value(ix, iy)[1])[:3]

    ex, ey = clamp_point(pinky.x, pinky.y, w, h)
    pinky_pt = np.array(point_cloud.get_value(ex, ey)[1])[:3]

    # Calcolo del vettore normale al piano formato dalle dita
    v1 = index_pt - wrist_pt
    v2 = pinky_pt - wrist_pt
    normal = np.cross(v1, v2)
    norm = np.linalg.norm(normal)
    if norm != 0:
        normal = normal / norm

    # L'angolo rispetto all'asse Y indica se il polso è orizzontale o verticale
    angle = float(np.degrees(np.arccos(np.clip(abs(normal[1]), -1.0, 1.0))))
    orientation_label = "orizzontale" if angle < 45 else "verticale"

    # Disegniamo sull'immagine l'orientamento rilevato
    cv2.putText(
        frame,
        f"{orientation_label}: {angle:.2f}",
        (cx, cy),
        cv2.FONT_HERSHEY_DUPLEX,
        0.5,
        (0, 0, 255),
        1,
    )

    # Prepariamo il messaggio JSON con posizione e angolo del polso
    message[label] = {
        "position": {
            "x": float(wrist_pt[0]),
            "y": float(wrist_pt[1]),
            "z": float(wrist_pt[2]),
        },
        "angle": angle,
    }
    return wrist_pt


def main() -> None:
    """Funzione principale dello script."""

    parser = argparse.ArgumentParser(description="ZED wrist orientation tracker")
    parser.add_argument(
        "--ip",
        default=None,
        help="Indirizzo IP di destinazione per i dati UDP",
    )
    parser.add_argument(
        "--port",
        type=int,
        default=None,
        help="Porta di destinazione per i dati UDP",
    )
    args = parser.parse_args()

    # Valori di default se non specificati da linea di comando o input
    ip = os.getenv("UDP_IP", "10.196.91.47")
    port = int(os.getenv("UDP_PORT", "5005"))

    # Se non passati come argomenti, chiediamoli all'utente
    if args.ip is None:
        # Richiediamo all'utente l'IP (invio per mantenere il default)
        ip_input = input(f"Inserisci l'IP di destinazione [{ip}]: ").strip()
        if ip_input:
            ip = ip_input
    else:
        ip = args.ip

    if args.port is None:
        # Analogamente chiediamo la porta
        port_input = input(f"Inserisci la porta di destinazione [{port}]: ").strip()
        if port_input:
            port = int(port_input)
    else:
        port = args.port

    # Inizializziamo le varie componenti del sistema
    sock = setup_udp(ip, port)
    zed, image_zed, point_cloud, runtime_params = setup_zed()
    mp_pose, mp_drawing, pose = setup_pose()

    # Messaggio di avvio
    print("Running wrist orientation tracking... Press 'q' to quit.")

    try:
        # Ciclo principale di acquisizione e processamento
        while True:
            if zed.grab(runtime_params) == sl.ERROR_CODE.SUCCESS:
                zed.retrieve_image(image_zed, sl.VIEW.LEFT)
                zed.retrieve_measure(point_cloud, sl.MEASURE.XYZ)

                # Otteniamo il frame corrente e lo convertiamo in RGB
                frame_rgba = image_zed.get_data()
                frame = cv2.cvtColor(frame_rgba, cv2.COLOR_RGBA2RGB)

                # Elaborazione Pose e struttura del messaggio da inviare
                results = pose.process(frame)
                message = {"left_wrist": None, "right_wrist": None}

                if results.pose_landmarks:
                    # Disegniamo i landmarks rilevati
                    mp_drawing.draw_landmarks(
                        frame, results.pose_landmarks, mp_pose.POSE_CONNECTIONS
                    )
                    # Lista completa dei punti rilevati
                    landmarks = results.pose_landmarks.landmark
                    h, w, _ = frame.shape

                    # Calcoliamo posizione e orientamento del polso sinistro
                    left_pt = process_wrist(
                        landmarks,
                        mp_pose.PoseLandmark.LEFT_WRIST.value,
                        mp_pose.PoseLandmark.LEFT_PINKY.value,
                        mp_pose.PoseLandmark.LEFT_INDEX.value,
                        "left_wrist",
                        w,
                        h,
                        point_cloud,
                        frame,
                        message,
                    )

                    # Idem per il polso destro
                    right_pt = process_wrist(
                        landmarks,
                        mp_pose.PoseLandmark.RIGHT_WRIST.value,
                        mp_pose.PoseLandmark.RIGHT_PINKY.value,
                        mp_pose.PoseLandmark.RIGHT_INDEX.value,
                        "right_wrist",
                        w,
                        h,
                        point_cloud,
                        frame,
                        message,
                    )

                    if left_pt is not None:
                        cv2.putText(
                            frame,
                            f"Polso Sinistro: {left_pt[0]:.2f}, {left_pt[1]:.2f}, {left_pt[2]:.2f}",
                            (10, 20),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5,
                            (255, 0, 0),
                            1,
                        )

                    if right_pt is not None:
                        cv2.putText(
                            frame,
                            f"Polso Destro: {right_pt[0]:.2f}, {right_pt[1]:.2f}, {right_pt[2]:.2f}",
                            (10, 40),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5,
                            (255, 0, 0),
                            1,
                        )

                # Inviamo il messaggio via UDP
                sock.sendto(json.dumps(message).encode("utf-8"), (ip, port))

                cv2.imshow("ZED Wrist Orientation", frame)
                if cv2.waitKey(1) & 0xFF == ord("q"):
                    break
    finally:
        # Chiusura ordinata di tutte le risorse
        cv2.destroyAllWindows()
        pose.close()
        zed.close()
        sock.close()


# Avviamo lo script solo se eseguito direttamente
if __name__ == "__main__":
    main()