import cv2
import numpy as np

def refine_object_detection(edge_image, corner_image, edge_weight=1, corner_weight=0):
    # Convert both images to the same data type
    edge_image = edge_image.astype(np.float32)
    corner_image = corner_image.astype(np.float32)

    # Combine edge and corner images to enhance object detection
    weighted_edge = edge_image * edge_weight
    weighted_corner = corner_image * corner_weight
    refined_image = cv2.addWeighted(weighted_edge, 1, weighted_corner, 1, 0)

    # Convert the refined image back to uint8 data type
    refined_image = np.clip(refined_image, 0, 255).astype(np.uint8)

    # Apply morphological operations for further refinement (optional)
    kernel = np.ones((3, 3), np.uint8)
    refined_image = cv2.morphologyEx(refined_image, cv2.MORPH_CLOSE, kernel)
    refined_image = cv2.morphologyEx(refined_image, cv2.MORPH_OPEN, kernel)

    return refined_image

if __name__ == "__main__":
    # Load edge and corner images (replace these with your actual edge and corner detection results)
    edge_image = cv2.imread("edge.jpg", cv2.IMREAD_GRAYSCALE)
    corner_image = cv2.imread("corner.jpg", cv2.IMREAD_GRAYSCALE)

    # Refine object detection based on edge and corner images
    refined_image = refine_object_detection(edge_image, corner_image)

    # Display the refined image
    cv2.imshow("Refined Object Detection", refined_image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
