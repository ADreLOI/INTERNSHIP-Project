"""Tracciamento dell'orientazione delle mani tramite ZED e MediaPipe.

Questo script funziona in maniera analoga a ``MP_BODY_Logic`` ma utilizza il
modello ``Hands`` di MediaPipe per rilevare i keypoint delle mani. I dati
raccolti (posizione del polso e orientamento della mano) vengono inviati tramite
UDP a un indirizzo configurabile.
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

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return sock


# --- Configurazione della ZED -----------------------------------------------
def setup_zed():
    """Inizializza la telecamera ZED e restituisce gli oggetti utili."""

    zed = sl.Camera()

    init_params = sl.InitParameters()
    init_params.camera_resolution = sl.RESOLUTION.HD720
    init_params.depth_mode = sl.DEPTH_MODE.PERFORMANCE

    if zed.open(init_params) != sl.ERROR_CODE.SUCCESS:
        raise RuntimeError("ZED Open Error")

    image_zed = sl.Mat()
    point_cloud = sl.Mat()

    runtime_params = sl.RuntimeParameters()
    return zed, image_zed, point_cloud, runtime_params


# --- Configurazione di MediaPipe Hands --------------------------------------
def setup_hands():
    """Restituisce l'istanza di Hands e gli strumenti di disegno."""

    mp_hands = mp.solutions.hands
    mp_drawing = mp.solutions.drawing_utils

    hands = mp_hands.Hands(
        static_image_mode=False,
        max_num_hands=2,
        min_detection_confidence=0.5,
        min_tracking_confidence=0.5,
    )

    return mp_hands, mp_drawing, hands


def clamp_point(x: float, y: float, w: int, h: int) -> tuple[int, int]:
    """Limita le coordinate normalizzate ai bordi del frame."""

    px = int(np.clip(x * w, 0, w - 1))
    py = int(np.clip(y * h, 0, h - 1))
    return px, py


def process_hand(
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
    """Calcola posizione e orientamento della mano specificata."""

    wrist = landmarks[wrist_id]
    pinky = landmarks[pinky_id]
    index = landmarks[index_id]

    cx, cy = clamp_point(wrist.x, wrist.y, w, h)
    wrist_pt = np.array(point_cloud.get_value(cx, cy)[1])[:3]

    ix, iy = clamp_point(index.x, index.y, w, h)
    index_pt = np.array(point_cloud.get_value(ix, iy)[1])[:3]

    px, py = clamp_point(pinky.x, pinky.y, w, h)
    pinky_pt = np.array(point_cloud.get_value(px, py)[1])[:3]

    v1 = index_pt - wrist_pt
    v2 = pinky_pt - wrist_pt
    normal = np.cross(v1, v2)
    norm = np.linalg.norm(normal)
    if norm != 0:
        normal = normal / norm

    angle = float(np.degrees(np.arccos(np.clip(abs(normal[1]), -1.0, 1.0))))
    orientation_label = "orizzontale" if angle < 45 else "verticale"

    cv2.putText(
        frame,
        f"{orientation_label}: {angle:.2f}",
        (cx, cy),
        cv2.FONT_HERSHEY_DUPLEX,
        0.5,
        (0, 0, 255),
        1,
    )

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

    parser = argparse.ArgumentParser(
        description="ZED hand orientation tracker"
    )
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

    ip = os.getenv("UDP_IP", "10.196.91.47")
    port = int(os.getenv("UDP_PORT", "5005"))

    if args.ip is None:
        ip_input = input(f"Inserisci l'IP di destinazione [{ip}]: ").strip()
        if ip_input:
            ip = ip_input
    else:
        ip = args.ip

    if args.port is None:
        port_input = input(f"Inserisci la porta di destinazione [{port}]: ").strip()
        if port_input:
            port = int(port_input)
    else:
        port = args.port

    sock = setup_udp(ip, port)
    zed, image_zed, point_cloud, runtime_params = setup_zed()
    mp_hands, mp_drawing, hands = setup_hands()

    print("Running hand orientation tracking... Press 'q' to quit.")

    try:
        while True:
            if zed.grab(runtime_params) == sl.ERROR_CODE.SUCCESS:
                zed.retrieve_image(image_zed, sl.VIEW.LEFT)
                zed.retrieve_measure(point_cloud, sl.MEASURE.XYZ)

                frame_rgba = image_zed.get_data()
                frame = cv2.cvtColor(frame_rgba, cv2.COLOR_RGBA2RGB)

                results = hands.process(frame)
                message = {"left_wrist": None, "right_wrist": None}

                if results.multi_hand_landmarks and results.multi_handedness:
                    for hand_landmarks, hand_handedness in zip(
                        results.multi_hand_landmarks,
                        results.multi_handedness,
                    ):
                        mp_drawing.draw_landmarks(
                            frame,
                            hand_landmarks,
                            mp_hands.HAND_CONNECTIONS,
                        )

                        label = hand_handedness.classification[0].label.lower()
                        landmarks = hand_landmarks.landmark
                        h, w, _ = frame.shape

                        hand_pt = process_hand(
                            landmarks,
                            mp_hands.HandLandmark.WRIST.value,
                            mp_hands.HandLandmark.PINKY_TIP.value,
                            mp_hands.HandLandmark.INDEX_FINGER_TIP.value,
                            f"{label}_wrist",
                            w,
                            h,
                            point_cloud,
                            frame,
                            message,
                        )

                        if hand_pt is not None:
                            y_offset = 20 if label == "left" else 40
                            cv2.putText(
                                frame,
                                f"Mano {label.capitalize()}: {hand_pt[0]:.2f}, {hand_pt[1]:.2f}, {hand_pt[2]:.2f}",
                                (10, y_offset),
                                cv2.FONT_HERSHEY_SIMPLEX,
                                0.5,
                                (255, 0, 0),
                                1,
                            )

                sock.sendto(json.dumps(message).encode("utf-8"), (ip, port))

                cv2.imshow("ZED Hand Orientation", frame)
                if cv2.waitKey(1) & 0xFF == ord("q"):
                    break
    finally:
        cv2.destroyAllWindows()
        hands.close()
        zed.close()
        sock.close()


if __name__ == "__main__":
    main()
