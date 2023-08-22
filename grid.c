#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>
typedef struct
{
    int originX;
    int originY;
    int gap;
    int point_gap;
    int click;
    int height;
    int width;
    cairo_t *cr;
    // double scale_factor;
} Grid;

//function prototypes
Grid *init_grid(cairo_t *cr, int gap, int click);
void create_grid(Grid *grid, int width, int height);
int transformX(Grid *grid, double x);
int transformY(Grid *grid, double y);
void grid_text(Grid *grid, char *value, double x, double y, int font_size);
void ui_text(cairo_t *cr, char *value, double x, double y, int f);
void point(Grid *grid, double x1, double y1);
void randomize_color(double *red, double *green, double *blue);
void createLine(Grid *grid, double x1, double y1, double x2, double y2);

Grid *init_grid(cairo_t *cr, int gap, int click)
{
    Grid *grid = calloc(1, sizeof(Grid));
    grid->cr = cr;
    grid->gap = gap;
    grid->click = click;
}

    // if(click%6 == 0 && gap != 60)
    // {
    //     gap = 60;
    // }
    //if position of a co-ordinate shifts from  5 to 10 pixel, scale factor is 2
    //if it shifts from 10 to 5, scale factor is 0.5
    // if (click>=0)
    // {
    // grid ->scale_factor  = click/6 + 1 ;
    // }
    // if (click<0)
    // {
    //     grid -> scale_factor = (double)1/(abs(click)/6 + 1);
    // }

void create_grid(Grid *grid, int width, int height) 
{
    grid->height = height;
    grid->width = width;
    cairo_t *cr = grid->cr;
    int gap = grid->gap;

    printf("gap: %d ", gap);
    printf("click: %d \n", grid->click);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1);

    double lineStroke = 0.2;
    cairo_set_line_width(cr, lineStroke);
    
    for (int i = 0; i <= height; i += gap)
    {
        cairo_move_to(cr, 0, i);
        cairo_line_to(cr, width, i);
        cairo_stroke(cr);
    }

    // **stroke of axis**//
    cairo_set_line_width(cr, 2);

    //X-axis
    int x_grid_count = (width / 2) / gap;
    int offset_x = (width / 2) % gap;
    int axis_x;
    if (offset_x > (gap / 2))
    {
        axis_x = (x_grid_count * gap) + gap;
    }
    else
    {
        axis_x = x_grid_count * gap;
    }

    cairo_move_to(cr, axis_x, 0);
    cairo_line_to(cr, axis_x, height);

    //y_axis
    int y_grid_count = (height / 2) / gap;
    int offset_y = (height / 2) % gap;
    int axis_y;
    if (offset_y > (gap / 2))
    {
        axis_y = (y_grid_count * gap) + gap;
    }
    else
    {
        axis_y = y_grid_count * gap;
    }

    grid->originX = axis_x;
    grid->originY = axis_y;

    cairo_move_to(cr, 0, axis_y);
    cairo_line_to(cr, width, axis_y);
    cairo_stroke(cr);
    cairo_set_line_width(cr, lineStroke);

    for (int i = 0; i < width; i += gap)
    {
        cairo_move_to(cr, i, 0);
        cairo_line_to(cr, i, height);
        cairo_stroke(cr);
    }

    //**handling texts**//

    //UI texts
    ui_text(grid->cr, "X", width - 20, (height / 2) - 20, 20);
    ui_text(grid->cr, "-X", 5, (height / 2) - 20, 20);
    ui_text(grid->cr, "Y", (width / 2), 20, 20);
    ui_text(grid->cr, "-Y", (width / 2), height - 20, 20);

    //grid_texts
    int step = 1;
    if (gap <= 20)
    {
        step = 4;
    }

    int index = 0;
    int stepIndex = 0;

 
    
    //x_axis positive
    for (double i = 0; i <= width / gap; i += 1)
    {
        char text[100];
        sprintf(text, "%d", index);
        if (stepIndex == step)
        {
            stepIndex = 0;
        }
        if (stepIndex == 0)
        {
            grid_text(grid, text, axis_x + i*gap -0.1, axis_y-1.0, 12);
        }
        stepIndex++;
        index++;
    }

    //x-axis negative
    index = 1;
    stepIndex = 1;

    for (double i = -1; i >= -1 * width / gap; i -= 1)
    {
        char text[100];
        sprintf(text, "-%d", index);

        if (stepIndex == step)
        {
            stepIndex = 0;
        }
        if (stepIndex == 0)
        {
            grid_text(grid, text, axis_x +i*gap - 0.1, axis_y-1.0, 12);
        }
        stepIndex++;
        index++;
    }

    //y-axis positive
    index = 1;
    stepIndex = 1;
    for (double i = 1; i <= height / gap; i += 1)
    {
        char t[100];
        sprintf(t, "%d", index);
        if (stepIndex == step)
        {
            stepIndex = 0;
        }
        if (stepIndex == 0)
        {
            grid_text(grid, t, axis_x + 1.0 , axis_y - i*gap + 0.1, 12);
        }
        stepIndex++;
        index++;
    }

    //y-axis negative
    index = 1;
    stepIndex = 1;
    for (double i = -1; i >= -1 * height / gap; i -= 1)
    {
        char t[100];
        sprintf(t, "-%d", index);
        if (stepIndex == step)
        {
            stepIndex = 0;
        }
        if (stepIndex == 0)
        {
            grid_text(grid, t, axis_x + 1.0 , axis_y - i*gap - 0.1, 12);
        }
        stepIndex++;
        index++;
    }
}

int transformX(Grid *grid, double x)
{
    return grid->originX + (x * grid->gap);
}

int transformY(Grid *grid, double y)
{
    return grid->originY + (y * grid->gap * -1);
}

//does not transfrom coordinates
void grid_text(Grid *grid, char *value, double x, double y, int font_size)
{
    cairo_t *cr = grid->cr;
    cairo_select_font_face(cr, "Purisa",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, font_size);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, value);
}

//transforms coordinates
void ui_text(cairo_t *cr, char *value, double x, double y, int f)
{
    cairo_select_font_face(cr, "Purisa",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, f);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, value);
}

void point(Grid *grid, double x1, double y1)
{
    cairo_t *cr = grid->cr;
    cairo_set_source_rgb(cr, 25 / 255.0, 163 / 255.0, 3 / 255.0);
    cairo_set_line_width(cr, 2);
    double d = 1;
    cairo_arc(cr, transformX(grid, x1), transformY(grid, y1), d, 0, 2 * G_PI);
    cairo_fill(cr);
}

void randomize_color(double *red, double *green, double *blue)
{
    // Color collection with 9 colors (dark tones)
    const double colors[9][3] = {
        {0.8, 0.2, 0.2}, // Red
        {0.2, 0.8, 0.2}, // Green
        {0.2, 0.2, 0.8}, // Blue
        {0.8, 0.8, 0.2}, // Yellow
        {0.8, 0.2, 0.8}, // Magenta
        {0.2, 0.8, 0.8}, // Cyan
        {0.6, 0.3, 0.1}, // Brown
        {0.5, 0.5, 0.5}, // Gray
        {0.1, 0.6, 0.3}  // Dark Green
    };

    // Set a seed for the random number generator.
    srand(time(NULL));

    // Generate a random index to choose a color from the collection.
    int random_index = rand() % 9;

    // Assign the selected color to the output pointers.
    *red = colors[random_index][0];
    *green = colors[random_index][1];
    *blue = colors[random_index][2];
}

void createLine(Grid *grid, double x1, double y1, double x2, double y2)
{
    cairo_t *cr = grid->cr;

    // cairo_set_source_rgb(cr, 255 / 255.0, 10 / 255.0, 3 / 255.0);

    cairo_set_line_width(cr, 3);
    cairo_move_to(cr, transformX(grid, x1), transformY(grid, y1));
    cairo_line_to(cr, transformX(grid, x2), transformY(grid, y2));
    cairo_stroke(cr);
}
