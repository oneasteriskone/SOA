
//This defines one space in the street.
typedef struct{
    int usable; // yes == 1 no == 0
    int busy;
    int carId;
    int direction; // E == 0 W == 1 N==2 S==3
    int model;// From 1 to 10
}StreetSpace;

void readUsableFile(StreetSpace spaceList[1230]);
