"""Tracciamento dei polsi con il body tracking della ZED.

Questo script replica la logica di MP_BODY_Logic ma utilizza il body
tracking nativo della ZED SDK.
"""
import argparse
import csv
import json
import logging
import os
import socket
import threading
import time
import warnings
from typing import Tuple

import cv2
import numpy as np
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
import pyzed.sl as sl

logging.basicConfig(level=logging.INFO)


def _showwarning(message, category, filename, lineno, file=None, line=None):
    logging.warning(f"{category.__name__}: {message}")


warnings.showwarning = _showwarning
warnings.filterwarnings("ignore", category=RuntimeWarning)


# --- ZED setup ---------------------------------------------------------------

def setup_zed() -> tuple[
    sl.Camera,
    sl.RuntimeParameters,
    sl.BodyTrackingRuntimeParameters,
    sl.Mat,
    sl.Bodies,
]:
    """Inizializza la ZED con il body tracking attivo."""
    zed = sl.Camera()
    init_params = sl.InitParameters()
    init_params.camera_resolution = sl.RESOLUTION.HD720
    init_params.depth_mode = sl.DEPTH_MODE.NEURAL
    init_params.coordinate_units = sl.UNIT.METER
    if zed.open(init_params) != sl.ERROR_CODE.SUCCESS:
        raise RuntimeError("ZED Open Error")

    pos_params = sl.PositionalTrackingParameters()
    if zed.enable_positional_tracking(pos_params) != sl.ERROR_CODE.SUCCESS:
        raise RuntimeError("Positional tracking enable error")

    bt_params = sl.BodyTrackingParameters()
    bt_params.detection_model = sl.BODY_TRACKING_MODEL.HUMAN_BODY_MEDIUM
    bt_params.enable_tracking = True
    bt_params.enable_body_fitting = True
    bt_params.body_format = sl.BODY_FORMAT.BODY_34
    if zed.enable_body_tracking(bt_params) != sl.ERROR_CODE.SUCCESS:
        raise RuntimeError("Body tracking enable error")

    runtime = sl.RuntimeParameters()
    body_runtime = sl.BodyTrackingRuntimeParameters()
    image = sl.Mat()
    bodies = sl.Bodies()
    return zed, runtime, body_runtime, image, bodies


# --- Utility -----------------------------------------------------------------

def compute_euler(quat: Tuple[float, float, float, float]) -> tuple[float, float, float]:
    """Converte un quaternion (x,y,z,w) in angoli Euler (pitch,yaw,roll)."""
    x, y, z, w = quat
    t0 = 2.0 * (w * x + y * z)
    t1 = 1.0 - 2.0 * (x * x + y * y)
    roll = np.degrees(np.arctan2(t0, t1))

    t2 = 2.0 * (w * y - z * x)
    t2 = np.clip(t2, -1.0, 1.0)
    pitch = np.degrees(np.arcsin(t2))

    t3 = 2.0 * (w * z + x * y)
    t4 = 1.0 - 2.0 * (y * y + z * z)
    yaw = np.degrees(np.arctan2(t3, t4))
    return pitch, yaw, roll


def to_millimeters(val: float) -> float:
    """Converte i metri in millimetri."""
    return float(val * 1000.0)


# --- Plotting ----------------------------------------------------------------

def plot_init():
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


def plot_update(time_data, pos_data, rot_data, plots) -> None:
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
    plots = plot_init()
    while not stop_event.is_set():
        with lock:
            t = time_vals.copy()
            p = [arr.copy() for arr in pos_vals]
            r = [arr.copy() for arr in rot_vals]
        plot_update(t, p, r, plots)
        time.sleep(0.05)


# --- Main --------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(description="ZED wrist orientation tracker")
    parser.add_argument("--ip", default=None, help="Indirizzo IP per i pacchetti UDP")
    parser.add_argument("--port", type=int, default=None, help="Porta UDP")
    args = parser.parse_args()

    ip = os.getenv("UDP_IP", "10.196.180.144")
    port = int(os.getenv("UDP_PORT", "5005"))

    if args.ip is None:
        user_ip = input(f"Inserisci l'IP di destinazione [{ip}]: ").strip()
        if user_ip:
            ip = user_ip
    else:
        ip = args.ip

    if args.port is None:
        user_port = input(f"Inserisci la porta di destinazione [{port}]: ").strip()
        if user_port:
            port = int(user_port)
    else:
        port = args.port

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    zed, runtime, body_runtime, image, bodies = setup_zed()

    csv_path = "wrist_zed.csv"
    new_csv = not os.path.exists(csv_path)
    csv_file = open(csv_path, "a", newline="")
    writer = csv.writer(csv_file)
    if new_csv:
        writer.writerow([
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
        ])

    time_vals: list[float] = []
    pos_vals = [[], [], []]
    rot_vals = [[], [], []]
    lock = threading.Lock()
    stop_event = threading.Event()
    plot_thread = threading.Thread(target=plot_worker, args=(stop_event, lock, time_vals, pos_vals, rot_vals))
    plot_thread.start()

    last_left_rot = [0.0, 0.0, 0.0]
    last_right_rot = [0.0, 0.0, 0.0]

    print("Running ZED wrist tracking... Press 'q' to quit.")

    try:
        while True:
            if zed.grab(runtime) == sl.ERROR_CODE.SUCCESS:
                zed.retrieve_image(image, sl.VIEW.LEFT)
                zed.retrieve_bodies(bodies, body_runtime)

                frame = cv2.cvtColor(image.get_data(), cv2.COLOR_RGBA2RGB)
                ts = time.time()
                message = {"timestamp": ts, "left_wrist": None, "right_wrist": None}

                left_pos = right_pos = None
                left_rot = right_rot = (np.nan, np.nan, np.nan)

                if bodies.body_list:
                    body = bodies.body_list[0]
                    h, w, _ = frame.shape
                    lw_idx = sl.BODY_34_PARTS.LEFT_WRIST.value
                    rw_idx = sl.BODY_34_PARTS.RIGHT_WRIST.value

                    if len(body.keypoint) > lw_idx:
                        kp = body.keypoint[lw_idx]
                        left_pos = tuple(to_millimeters(float(v)) for v in kp[:3])
                        rot_list = getattr(body, "local_rotation_per_joint", [])
                        if len(rot_list) > lw_idx:
                            raw_rot = tuple(np.degrees(rot_list[lw_idx]))
                        elif len(body.local_orientation_per_joint) > lw_idx:
                            raw_rot = compute_euler(body.local_orientation_per_joint[lw_idx])
                        else:
                            raw_rot = (np.nan, np.nan, np.nan)
                        rot = []
                        for i, val in enumerate(raw_rot):
                            if np.isfinite(val):
                                last_left_rot[i] = val
                            else:
                                val = last_left_rot[i]
                            rot.append(val)
                        left_rot = tuple(rot)
                        message["left_wrist"] = {
                            "position": {"x": left_pos[0], "y": left_pos[1], "z": left_pos[2]},
                            "rotation": {"pitch": left_rot[0], "yaw": left_rot[1], "roll": left_rot[2]},
                        }
                        cv2.putText(
                            frame,
                            f"Polso Sinistro: {left_pos[0]:.2f}, {left_pos[1]:.2f}, {left_pos[2]:.2f}",
                            (10, 20),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5,
                            (255, 0, 0),
                            1,
                        )
                        cpt = body.keypoint_2d[lw_idx]
                        cx, cy = int(cpt[0]), int(cpt[1])
                        cv2.putText(
                            frame,
                            f"p:{left_rot[0]:.1f} y:{left_rot[1]:.1f} r:{left_rot[2]:.1f}",
                            (cx, cy),
                            cv2.FONT_HERSHEY_DUPLEX,
                            0.5,
                            (0, 0, 255),
                            1,
                        )
                        with lock:
                            time_vals.append(ts)
                            for arr, val in zip(pos_vals, left_pos):
                                arr.append(val)
                            for arr, val in zip(rot_vals, left_rot):
                                arr.append(val)

                    if len(body.keypoint) > rw_idx:
                        kp = body.keypoint[rw_idx]
                        right_pos = tuple(to_millimeters(float(v)) for v in kp[:3])
                        rot_list = getattr(body, "local_rotation_per_joint", [])
                        if len(rot_list) > rw_idx:
                            raw_rot = tuple(np.degrees(rot_list[rw_idx]))
                        elif len(body.local_orientation_per_joint) > rw_idx:
                            raw_rot = compute_euler(body.local_orientation_per_joint[rw_idx])
                        else:
                            raw_rot = (np.nan, np.nan, np.nan)
                        rot = []
                        for i, val in enumerate(raw_rot):
                            if np.isfinite(val):
                                last_right_rot[i] = val
                            else:
                                val = last_right_rot[i]
                            rot.append(val)
                        right_rot = tuple(rot)
                        message["right_wrist"] = {
                            "position": {"x": right_pos[0], "y": right_pos[1], "z": right_pos[2]},
                            "rotation": {"pitch": right_rot[0], "yaw": right_rot[1], "roll": right_rot[2]},
                        }
                        cv2.putText(
                            frame,
                            f"Polso Destro: {right_pos[0]:.2f}, {right_pos[1]:.2f}, {right_pos[2]:.2f}",
                            (10, 40),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5,
                            (255, 0, 0),
                            1,
                        )
                        cpt = body.keypoint_2d[rw_idx]
                        cx, cy = int(cpt[0]), int(cpt[1])
                        cv2.putText(
                            frame,
                            f"p:{right_rot[0]:.1f} y:{right_rot[1]:.1f} r:{right_rot[2]:.1f}",
                            (cx, cy),
                            cv2.FONT_HERSHEY_DUPLEX,
                            0.5,
                            (0, 0, 255),
                            1,
                        )

                writer.writerow(
                    [
                        ts,
                        *(left_pos if left_pos else (np.nan, np.nan, np.nan)),
                        *(left_rot if left_pos else (np.nan, np.nan, np.nan)),
                        *(right_pos if right_pos else (np.nan, np.nan, np.nan)),
                        *(right_rot if right_pos else (np.nan, np.nan, np.nan)),
                    ]
                )

                sock.sendto(json.dumps(message).encode("utf-8"), (ip, port))

                cv2.imshow("ZED Wrist Tracking", frame)
                if cv2.waitKey(1) & 0xFF == ord("q"):
                    break
    finally:
        cv2.destroyAllWindows()
        zed.disable_body_tracking()
        zed.close()
        sock.close()
        csv_file.close()
        stop_event.set()
        plot_thread.join()
        plt.show()


if __name__ == "__main__":
    main()