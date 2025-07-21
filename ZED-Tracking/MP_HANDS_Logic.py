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
import time
import csv

import cv2
import mediapipe as mp
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
import logging
import warnings
import threading
import pyzed.sl as sl

logging.basicConfig(level=logging.INFO)


def _showwarning(message, category, filename, lineno, file=None, line=None):
    logging.warning(f"{category.__name__}: {message}")


warnings.showwarning = _showwarning

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


def compute_orientation(
    wrist_pt: np.ndarray, index_pt: np.ndarray, pinky_pt: np.ndarray
) -> tuple[float, float, float]:
    """Calcola pitch, yaw e roll della mano."""

    x_axis = index_pt - wrist_pt
    y_axis = pinky_pt - wrist_pt
    if np.linalg.norm(x_axis) == 0 or np.linalg.norm(y_axis) == 0:
        return 0.0, 0.0, 0.0
    x_axis = x_axis / np.linalg.norm(x_axis)
    y_axis = y_axis / np.linalg.norm(y_axis)
    z_axis = np.cross(x_axis, y_axis)
    if np.linalg.norm(z_axis) == 0:
        return 0.0, 0.0, 0.0
    z_axis = z_axis / np.linalg.norm(z_axis)
    y_axis = np.cross(z_axis, x_axis)

    R = np.column_stack((x_axis, y_axis, z_axis))
    sy = np.sqrt(R[0, 0] ** 2 + R[1, 0] ** 2)
    singular = sy < 1e-6
    if not singular:
        roll = np.degrees(np.arctan2(R[2, 1], R[2, 2]))
        pitch = np.degrees(np.arctan2(-R[2, 0], sy))
        yaw = np.degrees(np.arctan2(R[1, 0], R[0, 0]))
    else:
        roll = np.degrees(np.arctan2(-R[1, 2], R[1, 1]))
        pitch = np.degrees(np.arctan2(-R[2, 0], sy))
        yaw = 0.0
    return pitch, yaw, roll


def init_plots():
    """Inizializza le figure interattive."""

    plt.ion()

    fig_pos, ax_pos = plt.subplots()
    fig_pos.canvas.manager.set_window_title("Posizione")
    ax_pos.set_xlabel("Frame")
    ax_pos.set_ylabel("mm")
    ax_pos.set_title("Posizione")
    lp_x, = ax_pos.plot([], [], label="X")
    lp_y, = ax_pos.plot([], [], label="Y")
    lp_z, = ax_pos.plot([], [], label="Z")
    ax_pos.legend()

    fig_rot, ax_rot = plt.subplots()
    fig_rot.canvas.manager.set_window_title("Rotazione")
    ax_rot.set_xlabel("Frame")
    ax_rot.set_ylabel("deg")
    ax_rot.set_title("Rotazione")
    lr_p, = ax_rot.plot([], [], label="Pitch")
    lr_y, = ax_rot.plot([], [], label="Yaw")
    lr_r, = ax_rot.plot([], [], label="Roll")
    ax_rot.legend()

    return (fig_pos, ax_pos, [lp_x, lp_y, lp_z]), (fig_rot, ax_rot, [lr_p, lr_y, lr_r])


def update_plots(time_data, pos_data, rot_data, plots) -> None:
    """Aggiorna i grafici interattivi."""

    (fig_pos, ax_pos, pos_lines), (fig_rot, ax_rot, rot_lines) = plots

    for line, data in zip(pos_lines, pos_data):
        line.set_data(time_data, data)
    ax_pos.relim()
    ax_pos.autoscale_view()

    for line, data in zip(rot_lines, rot_data):
        line.set_data(time_data, data)
    ax_rot.relim()
    ax_rot.autoscale_view()

    for fig in (fig_pos, fig_rot):
        fig.canvas.draw()
        fig.canvas.flush_events()


def plot_worker(stop_event, lock, time_vals, pos_vals, rot_vals):
    """Thread per l'aggiornamento continuo dei grafici."""

    plots = init_plots()
    while not stop_event.is_set():
        with lock:
            t = time_vals.copy()
            p = [arr.copy() for arr in pos_vals]
            r = [arr.copy() for arr in rot_vals]
        update_plots(t, p, r, plots)
        time.sleep(0.05)


def plot_results(csv_path: str) -> None:
    """Legge il CSV e genera grafici delle posizioni e rotazioni."""

    df = pd.read_csv(csv_path)

    plt.figure()
    plt.plot(df["timestamp"], df["left_x"], label="left_x")
    plt.plot(df["timestamp"], df["left_y"], label="left_y")
    plt.plot(df["timestamp"], df["left_z"], label="left_z")
    plt.plot(df["timestamp"], df["right_x"], label="right_x")
    plt.plot(df["timestamp"], df["right_y"], label="right_y")
    plt.plot(df["timestamp"], df["right_z"], label="right_z")
    plt.xlabel("Time (s)")
    plt.ylabel("Position")
    plt.legend()
    plt.title("Traiettoria polsi")

    plt.figure()
    plt.plot(df["timestamp"], df["left_pitch"], label="left_pitch")
    plt.plot(df["timestamp"], df["left_yaw"], label="left_yaw")
    plt.plot(df["timestamp"], df["left_roll"], label="left_roll")
    plt.plot(df["timestamp"], df["right_pitch"], label="right_pitch")
    plt.plot(df["timestamp"], df["right_yaw"], label="right_yaw")
    plt.plot(df["timestamp"], df["right_roll"], label="right_roll")
    plt.xlabel("Time (s)")
    plt.ylabel("Degrees")
    plt.legend()
    plt.title("Rotazioni polsi")

    plt.show()


def process_hand(
    landmarks,
    wrist_id: int,
    pinky_id: int,
    index_id: int,
    w: int,
    h: int,
    point_cloud: sl.Mat,
):
    """Restituisce posizione 3D e orientamento della mano."""

    wrist = landmarks[wrist_id]
    pinky = landmarks[pinky_id]
    index = landmarks[index_id]

    cx, cy = clamp_point(wrist.x, wrist.y, w, h)
    wrist_pt = np.array(point_cloud.get_value(cx, cy)[1])[:3]

    ix, iy = clamp_point(index.x, index.y, w, h)
    index_pt = np.array(point_cloud.get_value(ix, iy)[1])[:3]

    px, py = clamp_point(pinky.x, pinky.y, w, h)
    pinky_pt = np.array(point_cloud.get_value(px, py)[1])[:3]

    return wrist_pt, compute_orientation(wrist_pt, index_pt, pinky_pt), (cx, cy)


def main() -> None:
    """Funzione principale dello script."""

    parser = argparse.ArgumentParser(
        description="ZED hand orientation tracker",
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

    ip = os.getenv("UDP_IP", "10.196.180.144")
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

    csv_path = "wrist_hands.csv"
    new_csv = not os.path.exists(csv_path)
    csv_file = open(csv_path, "a", newline="")
    writer = csv.writer(csv_file)
    if new_csv:
        writer.writerow(
            [
                "timestamp",
                "left_x",
                "left_y",
                "left_z",
                "left_pitch",
                "left_yaw",
                "left_roll",
                "right_x",
                "right_y",
                "right_z",
                "right_pitch",
                "right_yaw",
                "right_roll",
            ]
        )

    time_vals: list[float] = []
    pos_vals = [[], [], []]
    rot_vals = [[], [], []]
    lock = threading.Lock()
    stop_event = threading.Event()
    plot_thread = threading.Thread(
        target=plot_worker, args=(stop_event, lock, time_vals, pos_vals, rot_vals)
    )
    plot_thread.start()
    last_left_rot = [0.0, 0.0, 0.0]
    last_right_rot = [0.0, 0.0, 0.0]

    print("Running hand orientation tracking... Press 'q' to quit.")

    try:
        while True:
            if zed.grab(runtime_params) == sl.ERROR_CODE.SUCCESS:
                zed.retrieve_image(image_zed, sl.VIEW.LEFT)
                zed.retrieve_measure(point_cloud, sl.MEASURE.XYZ)

                frame_rgba = image_zed.get_data()
                frame = cv2.cvtColor(frame_rgba, cv2.COLOR_RGBA2RGB)

                ts = time.time()
                results = hands.process(frame)
                message = {
                    "timestamp": ts,
                    "left_wrist": None,
                    "right_wrist": None,
                }

                left_pt = right_pt = None
                left_rot = right_rot = (np.nan, np.nan, np.nan)

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

                        hand_pt, raw_rot, (hx, hy) = process_hand(
                            landmarks,
                            mp_hands.HandLandmark.WRIST.value,
                            mp_hands.HandLandmark.PINKY_TIP.value,
                            mp_hands.HandLandmark.INDEX_FINGER_TIP.value,
                            w,
                            h,
                            point_cloud,
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
                            rot = []
                            if label == "left":
                                target_last = last_left_rot
                            else:
                                target_last = last_right_rot
                            for i, val in enumerate(raw_rot):
                                if not np.isfinite(val):
                                    val = target_last[i]
                                else:
                                    target_last[i] = val
                                rot.append(val)
                            rot = tuple(rot)

                            message[f"{label}_wrist"] = {
                                "position": {
                                    "x": float(hand_pt[0]),
                                    "y": float(hand_pt[1]),
                                    "z": float(hand_pt[2]),
                                },
                                "rotation": {
                                    "pitch": rot[0],
                                    "yaw": rot[1],
                                    "roll": rot[2],
                                },
                            }

                            cv2.putText(
                                frame,
                                f"p:{rot[0]:.1f} y:{rot[1]:.1f} r:{rot[2]:.1f}",
                                (hx, hy),
                                cv2.FONT_HERSHEY_DUPLEX,
                                0.5,
                                (0, 0, 255),
                                1,
                            )

                            if label == "left":
                                left_pt, left_rot = hand_pt, rot
                                with lock:
                                    time_vals.append(ts)
                                    for arr, val in zip(pos_vals, hand_pt):
                                        arr.append(float(val))
                                    for arr, val in zip(rot_vals, rot):
                                        arr.append(val)
                            else:
                                right_pt, right_rot = hand_pt, rot

                writer.writerow(
                    [
                        ts,
                        *(left_pt if left_pt is not None else (np.nan, np.nan, np.nan)),
                        *(left_rot if left_pt is not None else (np.nan, np.nan, np.nan)),
                        *(right_pt if right_pt is not None else (np.nan, np.nan, np.nan)),
                        *(right_rot if right_rot is not None else (np.nan, np.nan, np.nan)),
                    ]
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
        csv_file.close()
        stop_event.set()
        plot_thread.join()


if __name__ == "__main__":
    main()
