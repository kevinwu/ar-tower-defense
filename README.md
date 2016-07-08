# AR Tower Defense Game
Augmented Reality Tower Defense Game for the Summer 2016 TUM AR Class Final Project (GitHub: https://github.com/kevinwu/ar-tower-defense). 

### Team
Markus Müller (sukram.mueller@tum.de), Valerian Backeberg (dienstbeer@gmail.com) and Kevin Wu (kevinwu_mtk@outlook.com). If you have questions concerning our project, just contact us! 

### Basic Idea
See the attached PDF presentation ("AR Tower Defense.pdf")

## Current State
### Demo
See a demo video here: https://www.dropbox.com/s/90i4itvhxo033gz/VID_20160708_233612.mp4?dl=0

### Summary
- From just two markers, we calculate all gaming field corners (the two missing corners are calculated via line intersections). The gaming field is then extracted from the camera image
- Self-designed OpenGL monster (in our case: a turret by default - can be changed to a "mannequin" character) walks steadily from one side of the gaming field ("start") to the other side ("exit"). When the exit is reached, the monster will start again.
- Black spots on the gaming field are recognized as walls -> monster will (in a very simple manner) try to circumcise a walls or start again from the start point if a wall is encountered

### Known Limitations
- Game only works from 2D bird's eye view because we had trouble converting a relative game field coordinate to an absolute camera coordinate when viewing the scene from other views
- Corner calculation by line intersection leads to unstable game field detection -> "flickering"
- Alignment of x- and y-coordinates of the monster figure is hard-coded. This alignment needs to be done for every game field basis (but: the alignment can be easily done with the trackhandlers in the "Settings" window)