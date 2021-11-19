#include "menu/menu.h"

#define STARSHIP {'\n',' ',' ','/','\\','\n','>','|',' ',' ','|','>','\n',' ',' ','\\','/','\n','\n'}

typedef struct allyship{
    Point pos;
    char sprite[sizeof(STARSHIP)];
} AllyShip;

typedef struct enemyship{
    Point pos;
    char sprite[sizeof(STARSHIP)];
} EnemyShip;

#define Y_HSEPARATOR 4

