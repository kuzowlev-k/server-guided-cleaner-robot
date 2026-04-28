import cv2
import numpy as np
from pyzbar import decode
import math

class QRDetector:
    def detect(self, frame):
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        _, binary = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
        objects = []
        decoded = decode(binary)
        for obj in decoded:
            data = obj.data.decode('utf-8').strip()
            if data in ('forward', 'back', 'goal'):
                points = obj.polygon
                if len(points) == 4:
                    pts = np.array([(p.x, p.y) for p in points], dtype=np.int32)
                    center = np.mean(pts, axis=0).astype(np.int32)
                    objects.append({'data': data, 'center': tuple(center), 'bbox': pts})
        return objects

def compute_angle_distance(forward_center, back_center, goal_center):
    robot_center = ((forward_center[0] + back_center[0])//2, (forward_center[1] + back_center[1])//2)
    dir_vec = (forward_center[0] - back_center[0], forward_center[1] - back_center[1])
    norm = math.hypot(dir_vec[0], dir_vec[1])
    if norm == 0:
        return 0, 0, robot_center
    dir_unit = (dir_vec[0]/norm, dir_vec[1]/norm)
    to_goal = (goal_center[0] - robot_center[0], goal_center[1] - robot_center[1])
    distance = math.hypot(to_goal[0], to_goal[1])
    if distance == 0:
        return 0, 0, robot_center
    to_goal_unit = (to_goal[0]/distance, to_goal[1]/distance)
    dot = dir_unit[0]*to_goal_unit[0] + dir_unit[1]*to_goal_unit[1]
    dot = max(-1.0, min(1.0, dot))
    angle_rad = math.acos(dot)
    cross = dir_unit[0]*to_goal_unit[1] - dir_unit[1]*to_goal_unit[0]
    if cross < 0:
        angle_rad = -angle_rad
    angle_deg = math.degrees(angle_rad)
    return angle_deg, distance, robot_center

def main():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("Cannot open camera")
        return
    detector = QRDetector()
    print("Looking for forward, back, goal QR codes. Press ESC to quit.")
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        qrs = detector.detect(frame)
        qr_dict = {item['data']: item for item in qrs}
        if 'forward' in qr_dict and 'back' in qr_dict and 'goal' in qr_dict:
            fwd = qr_dict['forward']['center']
            back = qr_dict['back']['center']
            goal = qr_dict['goal']['center']
            angle, distance, robot_center = compute_angle_distance(fwd, back, goal)
            cv2.circle(frame, fwd, 8, (0,255,0), -1)
            cv2.putText(frame, "F", (fwd[0]+5, fwd[1]), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,255,0), 2)
            cv2.circle(frame, back, 8, (0,255,0), -1)
            cv2.putText(frame, "B", (back[0]+5, back[1]), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,255,0), 2)
            cv2.circle(frame, goal, 8, (0,0,255), -1)
            cv2.putText(frame, "GOAL", (goal[0]+5, goal[1]), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,0,255), 2)
            cv2.circle(frame, robot_center, 10, (255,0,0), 2)
            cv2.arrowedLine(frame, back, fwd, (255,255,0), 2, tipLength=0.3)
            cv2.arrowedLine(frame, robot_center, goal, (0,255,255), 2, tipLength=0.2)
            cv2.putText(frame, f"Angle: {angle:.1f} deg", (10,30), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255,255,0), 2)
            cv2.putText(frame, f"Dist: {distance:.0f} px", (10,60), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255,255,0), 2)
            if angle > 5:
                cv2.putText(frame, ">>> TURN RIGHT", (10,95), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,200,200), 2)
            elif angle < -5:
                cv2.putText(frame, "<<< TURN LEFT", (10,95), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,200,200), 2)
            else:
                cv2.putText(frame, "^^^ GO STRAIGHT", (10,95), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,200,200), 2)
        else:
            cv2.putText(frame, "Need 3 QR codes: forward, back, goal", (10,30), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,0,255), 2)
        cv2.imshow("Robot Vision", frame)
        if cv2.waitKey(1) == 27:
            break
    cap.release()
    cv2.destroyAllWindows()
if __name__ == "__main__":
    main()
