#ifndef THREADVILLEDEFINITIONS_H
#define THREADVILLEDEFINITIONS_H

#define MAX_TRAVELS 10
#define MIN_TRAVELS 3
#define DESTINY_FILE "destiny.csv"
#define RESISTANCE_FILE "resistance.csv"
#define NEW_CAR_SPACES "newCarSpaces.csv"
#define SPACE_IDS_FILE "map.csv"
#define MAX_SPACES 1000
#define MAX_DESTINIES 2
#define MAX_LENGTH_DESTINY_LINE 50


enum VehicleType
{
    RED,
    BLUE,
    GREEN,
    BLACK,
    WHITE,
    YELLOW,
    AMBULANCE,
    BUS_ORANGE,
    BUS_PINK,
    BUS_BLUELIGHT,
    BUS_WHITE,
    BUS_BLACK,
    BUS_GRAY,
    BUS_RED,
    BUS_GREEN,
    BUS_BLUE,
    UNDEF
};

struct VehicleInfo
{
    enum VehicleType type;
    unsigned short travels;
    unsigned short* route;
    short currentSpace;
    int speed;
    int stopTime;
    int id;
};


struct BridgeInfo
{
    char* name;
    struct VehicleInfo* vehicles[6];
};
struct StreetSpaceInfo
{
    struct VehicleInfo* vehicle;
    short destiny[MAX_DESTINIES];
    struct BridgeInfo* bridge;
    unsigned short maxSpeed;
    unsigned short busStop;
    unsigned short underMaintenance;
    unsigned short resistance;
    unsigned short locked;
};

struct ThreadAttributes
{
    struct StreetSpaceInfo* graph;
    unsigned short graphLength;
    struct VehicleInfo* vehicle;
    int id;
};

#endif
