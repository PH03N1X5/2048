#ifndef _2048_H
#define _2048_H

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifndef TABLE_SIZE
# define TABLE_SIZE 4
#endif //TABLE_SIZE

typedef enum { left, down, up, right, } Direction;

void _2048_swipe            (int table[][TABLE_SIZE], Direction d); /* Call this function once you know the direction the user pressed */

void _2048_move             (int table[][TABLE_SIZE], int ref[][TABLE_SIZE], int x, int y, Direction d);
void _2048_merge            (int table[][TABLE_SIZE], int ref[][TABLE_SIZE], int x, int y, Direction d);
bool _2048_table_has_space  (int table[][TABLE_SIZE]);
bool _2048_table_has_changed(int table[][TABLE_SIZE], int old[][TABLE_SIZE]);
void _2048_table_spawn      (int table[][TABLE_SIZE]);


#ifdef _2048_IMPLEMENTATION

int old[TABLE_SIZE][TABLE_SIZE];

void _2048_swipe(int table[][TABLE_SIZE], Direction d)
{
    int x0, xn, dx;
    int y0, yn, dy;

    switch(d)
    {
        case left:  
            x0 = 0           ; xn = +TABLE_SIZE; dx = +1;
            y0 = 1           ; yn = +TABLE_SIZE; dy = +1;
            break            ;  
  
        case right:   
            x0 = 0           ; xn = +TABLE_SIZE; dx = +1;
            y0 = TABLE_SIZE-2; yn = -1         ; dy = -1;
            break            ;  
  
        case down:    
            x0 = TABLE_SIZE-2; xn = -1         ; dx = -1;
            y0 = 0           ; yn = +TABLE_SIZE; dy = +1;
            break            ;  
  
        case up:      
            x0 = 1           ; xn = +TABLE_SIZE; dx = +1;
            y0 = 0           ; yn = +TABLE_SIZE; dy = +1;
            break;

        default: break; //unreachable
    }

    // we need this to check if the table has changed

    memcpy(old, table, TABLE_SIZE*TABLE_SIZE*sizeof(int));
    // ref is used to not merge a recently merged cell with another
    // as an example we can image we swipe right on the following:
    // * 4 4 2 2
    // we want the output to be 
    // * 0 0 8 4
    // not 
    // * 0 0 4 8
    int ref[TABLE_SIZE][TABLE_SIZE];

    for (int x = x0; (dx > 0 ? x < xn : x > xn); x += dx) 
        for (int y = y0; (dy > 0 ? y < yn : y > yn); y += dy) 
            if (table[x][y]) 
                _2048_merge(table, ref, x, y, d); 
    memset(ref, 0, TABLE_SIZE*TABLE_SIZE*sizeof(int));

    for (int x = x0; (dx > 0 ? x < xn : x > xn); x += dx) 
        for (int y = y0; (dy > 0 ? y < yn : y > yn); y += dy) 
            if (table[x][y]) 
                _2048_move(table, ref, x, y, d);

    if (_2048_table_has_changed(old, table))
        _2048_table_spawn(table);

}

void _2048_merge(int table[][TABLE_SIZE], int ref[][TABLE_SIZE], int x, int y, Direction d)
{
    int ox, oy; // offset
    int ax, ay; // activate x y or k
    int k0, kn, dk; // for(int k = k0; k < kn; k += dk)
    switch(d)
    {
        case left:  ax =  1; ay =  0; ox = x  ;   oy = y-1;     k0 = y-1;  kn = -1         ;  dk = -1; break;
        case down:  ax =  0; ay =  1; ox = x+1;   oy = y  ;     k0 = x+1;  kn = +TABLE_SIZE;  dk = +1; break;
        case up:    ax =  0; ay =  1; ox = x-1;   oy = y  ;     k0 = x-1;  kn = -1         ;  dk = -1; break;
        case right: ax =  1; ay =  0; ox = x  ;   oy = y+1;     k0 = y+1;  kn = +TABLE_SIZE;  dk = +1; break;
        default: break;
    }

    for (int k = k0; (dk > 0 ? k < kn: k > kn); k += dk)
    {
        int cx  = ax*ox + ay*k;
        int cy  = ay*oy + ax*k;
        if (table[cx][cy])
        {
            if (table[cx][cy] == table[x][y] && ref[cx][cy] != 1)
            {
                table[cx][cy] += table[x][y];
                ref[cx][cy] = 1;
                table[x][y] = 0;
                return;
            }
            return;
        }
    }
    return;
}

void _2048_move(int table[][TABLE_SIZE], int ref[][TABLE_SIZE], int x, int y, Direction d)
{
    int ox, oy; // offset
    int ax, ay; // activate x y or k
    int k0, kn, dk; // for(int k = k0; k < kn; k += dk)
    switch(d)
    {
        case left:  ax =  1; ay =  0; ox = x  ; oy = y-1;     k0 = y-1; kn = -1         ; dk = -1; break;
        case down:  ax =  0; ay =  1; ox = x+1; oy = y  ;     k0 = x+1; kn = +TABLE_SIZE; dk = +1; break;
        case up:    ax =  0; ay =  1; ox = x-1; oy = y  ;     k0 = x-1; kn = -1         ; dk = -1; break;
        case right: ax =  1; ay =  0; ox = x  ; oy = y+1;     k0 = y+1; kn = +TABLE_SIZE; dk = +1; break;
        default: break;
    }

    for (int k = k0; (dk > 0 ? k < kn + 1 : k > kn - 1); k += dk)
    {
        int cx  = ax*ox + ay*k;
        int cy  = ay*oy + ax*k;
        int cxk = ax*ox + ay*(k-dk);
        int cyk = ay*oy + ax*(k-dk);
        /* if (obstacle) */
        // obstacle may mean: 
        // 1. cell right after current cell
        // 2. cell up until the last legal cell (including)
        // 3. outside boundry cell (imaginary)
        if (table[cx][cy])
        {
            table[cxk][cyk] = table[x][y]; // 2
            if (k != k0) // 1
                table[x][y] = 0;
            return;
        }
        if (k == kn) // 3
        {
            table[cxk][cyk] = table[x][y];
            table[x][y] = 0;
            return;
        }
    }
    return;
}

bool _2048_table_has_changed(int table[][TABLE_SIZE], int old[][TABLE_SIZE])
{
    for (int i = 0; i < TABLE_SIZE; i++)
        for (int j = 0; j < TABLE_SIZE; j++)
            if (table[i][j] != old[i][j])
                return true;
    return false;
}

bool _2048_table_has_space(int table[][TABLE_SIZE])
{
    for (int i = 0; i < TABLE_SIZE; i++)
        for (int j = 0; j < TABLE_SIZE; j++)
            if (table[i][j] == 0)
                return true;
    return false;
}

void _2048_table_spawn(int table[][TABLE_SIZE])
{
    bool generated = false;
    if (_2048_table_has_space(table))
    {
        while(!generated)
        {
            int x = rand()%TABLE_SIZE;
            int y = rand()%TABLE_SIZE;
            if (!table[x][y])
            {
                int number;
                int r = rand();
                if (r % 10 < 9 )
                    number = 2;
                else
                    number = 4;
                table[x][y] = number;
                generated = true;
            }
        }
    }
}

#endif //_2048_IMPLEMENTATION

#endif // _2048_H
