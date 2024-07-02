import cv2
import numpy as np
import argparse
import random
import os

image_path = ""

def main(args):
	im = cv2.imread(args.rgb)
	image_height, image_width = im.shape[1], im.shape[0]

	with open(args.txt, 'r') as f:
		labels = f.readlines()
		print('readlines',len(labels))

		if not os.path.exists("../masks/" + image_path):
			os.mkdir("../masks/" + image_path)
	i=0
	for label in labels:
		print('label: ', len(label))
		print('class', label[0])

		class_ = label[0]

		print('label type', type(label))
		print('CLASS:\n', label[0])
		label = label[2:-2].split(' ')
		# print(label)
		for k in label:
			k = float(k)
		c = [float(k) for k in label]
		print('label type after split',c)
		c = np.array(c).reshape((-1, 1, 2))
		c[:,:,0] = c[:,:,0] * image_height
		c[:,:,1] = c[:,:,1] * image_width
		c = c.astype(np.int32)
		print('c shape',c.shape)
		print(c)
		print()

		mask_bin = np.zeros_like(im)
		# c = np.array([[[int(pair[0]*im.shape[1]), int(pair[1]*im.shape[0])] for pair in det] for det in c])
		# image = np.zeros((image_height, image_width, 3), dtype=np.uint8)

		# Draw the points as a contour
		cv2.drawContours(im, [c], -1, (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)), thickness=2)
		cv2.drawContours(mask_bin, [c], -1, (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)), thickness=cv2.FILLED)

		# c = [[ann.split(' ')] for ann in label]
		# c = [[[float(k) for k in ann] for ann in i] for i in label]

		#for det in c:
		#    print('det', det)

		# c = [[[np.reshape(np.array(det[1:]), (-1,2))] for det in lista] for lista in c]
		# print(len(c))
		# print(len(c[0]))
		# print(len(c[0][0]))
		# print(len(c[0][0][0]))
		# print(len(c[0][0][0][0]))

		# c = np.squeeze(np.squeeze(c, 1), 1)
		#print(
		# c = [[[int(c[i][k][0]*im.shape[0]), int(c[i][k]*im.shape[1])] for k in len(det)] for i in len(c)]
		c = [[[int(pair[0]*im.shape[1]), int(pair[1]*im.shape[0])] for pair in det] for det in c]

		# print(c[0][1])
		# print(c[0]) 
		
		mask_true = cv2.drawContours(im, [np.array(c)], 0, (255,0,0), 3 )
		mask_bin = cv2.drawContours(mask_bin, [np.array(c)], 0, (255,0,0), 3 )
		# print(mask_true)
		#cv2.imwrite('/home/elena/repos/yolov7/annotation2mask/paper8.jpg', mask_true)
		#cv2.imshow('check annotations', mask_true)
		#cv2.waitKey(0)

		#cv2.imshow('check mask', mask_bin)
		#cv2.waitKey(0)
		cv2.imwrite('../masks/' + image_path + '/' + f'{image_path}_mask_{i}_{class_}.png', mask_bin)
		i+=1


		# Find the contours
		# contours, _ = cv2.findContours(gray, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

		# Create an empty binary mask with the same dimensions as the image
		# mask = np.zeros((image_width, image_height), dtype=np.uint8)

		# # Draw the contours on the mask
		# mask = cv2.drawContours(mask, [np.array(c)], -1, (200), thickness=cv2.FILLED)
		# cv2.imwrite('binary_mask.png', mask)

		# # cv2.drawContours(mask, [np.array(c)], -1, (255), thickness=cv2.FILLED)
		# # mask = np.ones((image_width, image_height,3))
		# # cv2.drawContours(im*0, [np.array(c)], 0, (0,255,0), 3) 
		# cv2.imshow('check binary mask', mask)
		# cv2.waitKey(0)

if __name__ == "__main__":
	# Get the path to the folder containing the txt files
	txt_folder = '.'

	# Iterate over all the files in the folder
	for filename in os.listdir(txt_folder):
		# Check if the file is a txt file
		if filename.endswith('.txt'):
			# Get the full path to the txt file
			txt_path = os.path.join(txt_folder, filename)
			jpg_path = os.path.join(txt_folder, os.path.splitext(filename)[0] + '.jpg')
			
			parser = argparse.ArgumentParser(description="Description of your script")

			# Add command-line arguments
			parser.add_argument("--rgb", type=str, help="rgb filename path", default=jpg_path)
			parser.add_argument("--txt", type=str, help="txt filename path", default=txt_path)

			# Parse the command-line arguments
			args = parser.parse_args()

			image_path = os.path.splitext(filename)[0]

			# Call the main function with the parsed arguments
			main(args)
