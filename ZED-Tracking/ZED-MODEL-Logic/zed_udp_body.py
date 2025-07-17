import argparse
import json
import os
import socket
from typing import List

import cv2
import numpy as np
import pyzed.sl as sl
import cv_viewer.tracking_viewer as cv_viewer
import math


def quat_to_euler(x: float, y: float, z: float, w: float) -> dict:
    """Convert quaternion to Euler angles in degrees."""
    t0 = 2.0 * (w * x + y * z)
    t1 = 1.0 - 2.0 * (x * x + y * y)
    roll = math.degrees(math.atan2(t0, t1))

    t2 = 2.0 * (w * y - z * x)
    t2 = max(min(t2, 1.0), -1.0)
    pitch = math.degrees(math.asin(t2))

    t3 = 2.0 * (w * z + x * y)
    t4 = 1.0 - 2.0 * (y * y + z * z)
    yaw = math.degrees(math.atan2(t3, t4))

    return {"roll": roll, "pitch": pitch, "yaw": yaw}

def setup_udp(ip: str, port: int) -> socket.socket:
    """Create a UDP socket."""
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return sock


def setup_zed():
    """Initialize the ZED camera with body tracking enabled."""
    zed = sl.Camera()

    init_params = sl.InitParameters()
    init_params.camera_resolution = sl.RESOLUTION.HD720
    init_params.coordinate_units = sl.UNIT.METER
    init_params.depth_mode = sl.DEPTH_MODE.NEURAL

    if zed.open(init_params) != sl.ERROR_CODE.SUCCESS:
        raise RuntimeError("Failed to open ZED camera")

    positional_tracking_parameters = sl.PositionalTrackingParameters()
    zed.enable_positional_tracking(positional_tracking_parameters)

    body_param = sl.BodyTrackingParameters()
    body_param.enable_tracking = True
    body_param.enable_body_fitting = False
    body_param.detection_model = sl.BODY_TRACKING_MODEL.HUMAN_BODY_FAST
    body_param.body_format = sl.BODY_FORMAT.BODY_18

    zed.enable_body_tracking(body_param)

    runtime = sl.BodyTrackingRuntimeParameters()
    runtime.detection_confidence_threshold = 40

    image = sl.Mat()
    bodies = sl.Bodies()

    return zed, runtime, image, bodies, body_param


def bodies_to_dict(bodies: sl.Bodies) -> List[dict]:
    """Convert detected bodies into a serialisable structure."""
    bodies_msg = []
    for body in bodies.body_list:
        joints = []
        for j, kp in enumerate(body.keypoint):
            pos = {"x": float(kp[0]), "y": float(kp[1]), "z": float(kp[2])}
            quat = None
            euler = None
            if len(body.local_orientation_per_joint) > j:
                q = body.local_orientation_per_joint[j]
                quat = {
                    "x": float(q[0]),
                    "y": float(q[1]),
                    "z": float(q[2]),
                    "w": float(q[3]),
                }
                euler = quat_to_euler(q[0], q[1], q[2], q[3])
            joints.append({"index": j, "position": pos, "orientation": quat, "rotator": euler})
        bodies_msg.append({"id": body.id, "joints": joints})
    return bodies_msg


def main() -> None:
    parser = argparse.ArgumentParser(description="ZED body tracking UDP streamer")
    parser.add_argument("--ip", default=None, help="UDP destination IP")
    parser.add_argument("--port", type=int, default=None, help="UDP destination port")
    args = parser.parse_args()

    ip = os.getenv("UDP_IP", "10.196.91.47")
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

    sock = setup_udp(ip, port)
    zed, runtime, image, bodies, body_param = setup_zed()

    camera_info = zed.get_camera_information()
    display_resolution = sl.Resolution(
        min(camera_info.camera_configuration.resolution.width, 1280),
        min(camera_info.camera_configuration.resolution.height, 720),
    )
    image_scale = [
        display_resolution.width / camera_info.camera_configuration.resolution.width,
        display_resolution.height / camera_info.camera_configuration.resolution.height,
    ]

    print("Running ZED body tracking... Press 'q' to quit.")

    key_wait = 10
    try:
        while True:
            if zed.grab() == sl.ERROR_CODE.SUCCESS:
                zed.retrieve_image(image, sl.VIEW.LEFT, sl.MEM.CPU, display_resolution)
                zed.retrieve_bodies(bodies, runtime)

                msg = {"bodies": bodies_to_dict(bodies)}
                sock.sendto(json.dumps(msg).encode("utf-8"), (ip, port))

                frame = image.get_data()
                cv_viewer.render_2D(frame, image_scale, bodies.body_list, True, body_param.body_format)

                if bodies.body_list:
                    first_body = bodies.body_list[0]
                    if first_body.local_orientation_per_joint:
                        root_rot = quat_to_euler(*first_body.local_orientation_per_joint[0])
                        txt = f"Root P:{root_rot['pitch']:.1f} Y:{root_rot['yaw']:.1f} R:{root_rot['roll']:.1f}"
                        cv2.putText(frame, txt, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)

                cv2.imshow("ZED Body Tracking", frame)
                key = cv2.waitKey(key_wait)
                if key == ord("q"):
                    break
    finally:
        cv2.destroyAllWindows()
        zed.disable_body_tracking()
        zed.disable_positional_tracking()
        zed.close()
        sock.close()


if __name__ == "__main__":
    main()
