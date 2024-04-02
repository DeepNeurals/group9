import cv2
import numpy as np
import os

def compute_depth(optical_flow):
    # Compute magnitude of optical flow vectors
    flow_magnitude = np.sqrt(optical_flow[..., 0]**2)

    # Scale magnitude to range [0, 255]
    magnitude_scaled = cv2.normalize(flow_magnitude, None, 0, 255, cv2.NORM_MINMAX)

    # Invert magnitude to represent depth (farther objects have smaller magnitudes)
    depth = 255 - magnitude_scaled
    return depth.astype(np.uint8)

if __name__ == "__main__":
    # Input and output directories
    input_dir = "./images_orange/"
    output_dir = "./output_orange_horizontal/"

    # Create the output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)

    # Get a list of files in the input directory
    filenames = [filename for filename in os.listdir(input_dir) if filename.endswith('.jpg')]
    filenames.sort()

    # Loop over consecutive pairs of images
    for i in range(len(filenames) - 1):
        # Load two consecutive frames
        frame1 = cv2.imread(os.path.join(input_dir, filenames[i]))
        frame2 = cv2.imread(os.path.join(input_dir, filenames[i + 1]))

        if frame1 is None or frame2 is None:
            print("Unable to open one or both of the images!")
            break
        
        # Convert frames to grayscale
        prvs = cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY)
        next = cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY)

        # Compute optical flow
        optical_flow = cv2.calcOpticalFlowFarneback(prvs, next, None, 0.6, 4, 10, 5, 7, 1.2, 0)

        # Compute depth ratio
        depth_ratio = compute_depth(optical_flow)

        # Save the result
        output_filename = os.path.join(output_dir, f"result_{i}.jpg")
        cv2.imwrite(output_filename, depth_ratio)
        print("Processed", output_filename)
