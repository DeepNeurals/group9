import cv2
import os
import numpy as np
from sklearn.cluster import DBSCAN

def compute_depth(optical_flow):
    # Compute magnitude of optical flow vectors
    flow_magnitude = np.sqrt(optical_flow[..., 0]**2 + optical_flow[..., 1]**2)

    # Scale magnitude to range [0, 255]
    magnitude_scaled = cv2.normalize(flow_magnitude, None, 0, 255, cv2.NORM_MINMAX)

    # Invert magnitude to represent depth (farther objects have smaller magnitudes)
    depth = 255 - magnitude_scaled
    return depth.astype(np.uint8)

if __name__ == "__main__":
    # Input and output directories
    input_dir = "./images_pole/"
    output_dir = "./FAST_blue/"

    # Create the output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)

    # Get a list of files in the input directory
    filenames = [filename for filename in os.listdir(input_dir) if filename.endswith('.jpg')]
    filenames.sort()

    # Create a FAST object
    fast = cv2.FastFeatureDetector_create()

    # Loop over consecutive pairs of images
    for i in range(len(filenames) - 1):
        # Load two consecutive frames
        frame1 = cv2.imread(os.path.join(input_dir, filenames[i]))
        frame2 = cv2.imread(os.path.join(input_dir, filenames[i + 1]))
        
        # Convert frame to grayscale
        grey1 = cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY)
        grey2 = cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY)

        # Detect keypoints using FAST
        keypoints1 = fast.detect(grey1, None)
        keypoints2 = fast.detect(grey2, None)

        # Draw keypoints on the image
        keypoints_img1 = cv2.drawKeypoints(frame1, keypoints1, None, color=(255, 0, 0)) # Blue color for keypoints
        keypoints_img2 = cv2.drawKeypoints(frame2, keypoints2, None, color=(255, 0, 0)) # Blue color for keypoints

        # Save the result
        output_filename1 = os.path.join(output_dir, f"result_{i}_1.jpg")
        cv2.imwrite(output_filename1, keypoints_img1)
        print("Processed", output_filename1)

        output_filename2 = os.path.join(output_dir, f"result_{i}_2.jpg")
        cv2.imwrite(output_filename2, keypoints_img2)
        print("Processed", output_filename2)
