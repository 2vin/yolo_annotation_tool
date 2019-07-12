## Yolo Annotation Tool (YAT)
(Annotation tool for YOLO in opencv)

This is an open-source tool for reearchers to annotate dataset for YOLO quickly in opencv. If you are familiar with opencv c++, you can directly use this tool to annotate data and create object detection dataset for the deep learning framework - YOLO. 

## How to use this tool?
1. Clone the repository from https://github.com/2vin/yolo_annotation
2. Compile the program using the instructions in the file 'compile.sh'
3. Once compiled, reset the counter in '/data/index.txt' to '0' to start with.
4. Run the executable as mentioned in the file 'compile.sh'
5. The annotation window will open-
    Press 'd' key to go to next frame.
    Press 'w' key to jump next 30 frames.
    Press 'x' key to reset annotations in the frame.
    Press 's' key to save the annotations in the output folder.
    Press 'Esc' key to quit the program.
6. Repeat the process for new video files, and keep track of the counter in '/data/index.txt'

## Screenshot
![YAT](https://user-images.githubusercontent.com/38634222/61153312-77382380-a508-11e9-9449-32e1995e8612.png)
