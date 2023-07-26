#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

typedef struct
{
    int originX;
    int originY;
    int gap;
    int height;
    int width;
    cairo_t *cr;
} Grid;

Grid *init_grid(cairo_t *cr, int gap);
void create_grid(Grid *grid, int width, int height);
int transformX(Grid *grid, double x);
int transformY(Grid *grid, double y);
void text(Grid *grid, char *value, double x, double y, int f);
void text_open(cairo_t *cr, char *value, double x, double y, int f);
void point(Grid *grid, double x1, double y1);
void createLine(Grid *grid, double x1, double y1, double x2, double y2);

Grid *init_grid(cairo_t *cr, int gap)
{
    Grid *grid = calloc(1, sizeof(Grid));
    grid->cr = cr;
    grid->gap = gap;
}

void create_grid(Grid *grid, int width, int height)
{
    grid->height = height;
    grid->width = width;
    cairo_t *cr = grid->cr;
    int gap = grid->gap;

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

    // stroke of axis
    cairo_set_line_width(cr, 2);
    int gapperW = (width / 2) / gap;
    int modW = (width / 2) % gap;
    int gW;
    if (modW > (gap / 2))
    {
        gW = (gapperW * gap) + gap;
    }
    else
    {
        gW = gapperW * gap;
    }

    cairo_move_to(cr, gW, 0);
    cairo_line_to(cr, gW, height);

    int gapperH = (height / 2) / gap;
    int modH = (height / 2) % gap;
    int gH;
    if (modH > (gap / 2))
    {
        gH = (gapperH * gap) + gap;
    }
    else
    {
        gH = gapperH * gap;
    }

    grid->originX = gW;
    grid->originY = gH;

    cairo_move_to(cr, 0, gH);
    cairo_line_to(cr, width, gH);
    cairo_stroke(cr);
    cairo_set_line_width(cr, lineStroke);

    for (int i = 0; i < width; i += gap)
    {
        cairo_move_to(cr, i, 0);
        cairo_line_to(cr, i, height);
        cairo_stroke(cr);
    }

    text_open(grid->cr, "X", width - 20, (height / 2) - 20, 20);
    text_open(grid->cr, "-X", 5, (height / 2) - 20, 20);
    text_open(grid->cr, "Y", (width / 2), 20, 20);
    text_open(grid->cr, "-Y", (width / 2), height - 20, 20);

    int index = 0;
    int step = 1;
    if (gap <= 20)
    {
        step = 2;
    }
    if (gap <= 10)
    {
        step = 4;
    }
    int isStep = 1;
    int stepIndex = 0;
    for (double i = 0; i <= width / gap; i += 1)
    {
        char t[100];
        sprintf(t, "%d", index);
        if (stepIndex == step)
        {
            stepIndex = 0;
        }
        if (stepIndex == 0)
        {
            text(grid, t, i - 0.3, -0.4, 12);
        }
        stepIndex++;
        index++;
    }
    index = 1;
    isStep = 1;
    stepIndex = 1;

    for (double i = -1; i >= -1 * width / gap; i -= 1)
    {
        char t[100];
        sprintf(t, "-%d", index);
        if (stepIndex == step)
        {
            stepIndex = 0;
        }
        if (stepIndex == 0)
        {
            text(grid, t, i - 0.1, -0.4, 12);
        }
        stepIndex++;
        index++;
    }

    index = 1;
    isStep = 1;
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
            text(grid, t, -0.4, i - 0.1, 12);
        }
        stepIndex++;
        index++;
    }
    index = 1;
    isStep = 1;
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
            text(grid, t, -0.4, i - 0.1, 12);
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

void text(Grid *grid, char *value, double x, double y, int f)
{
    cairo_t *cr = grid->cr;
    cairo_select_font_face(cr, "Purisa",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, f);
    cairo_move_to(cr, transformX(grid, x), transformY(grid, y));
    cairo_show_text(cr, value);
}

void text_open(cairo_t *cr, char *value, double x, double y, int f)
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
    // Set a seed for the random number generator.
    srand(time(NULL));

    // Generate random values for the RGB components.
    *red = (rand() % 256) / 255.0;   // Random value between 0 and 1
    *green = (rand() % 256) / 255.0; // Random value between 0 and 1
    *blue = (rand() % 256) / 255.0;  // Random value between 0 and 1

    // Calculate the brightness of the color.
    double brightness = (*red + *green + *blue) / 3.0;

    // If the brightness is too low, increase it slightly to add contrast.
    if (brightness < 0.3)
    {
        double diff = 0.3 - brightness;
        *red += diff;
        *green += diff;
        *blue += diff;
    }
}

void createLine(Grid *grid, double x1, double y1, double x2, double y2)
{
    cairo_t *cr = grid->cr;
    if (transformY(grid, y1) > 0)
    {

        cairo_set_source_rgb(cr, 255 / 255.0, 10 / 255.0, 3 / 255.0);

        cairo_set_line_width(cr, 3);
        cairo_move_to(cr, transformX(grid, x1), transformY(grid, y1));
        cairo_line_to(cr, transformX(grid, x2), transformY(grid, y2));
        cairo_stroke(cr);
    }
}
