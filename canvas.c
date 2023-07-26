#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "grid.c"
#include "compiler.c"

static void redraw_drawing_area(gpointer user_data);

GtkWidget *areaGlobal;
GtkWidget *eqnEntry;

void applyCSS(GtkWindow *win)
{

    GdkDisplay *display;
    display = gtk_widget_get_display(GTK_WIDGET(win));
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "style.css");
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

int gap = 40;

double f(double x);

double f(double x)
{
    return x * x * x;
}
double f2(double x)
{
    return x * x;
}

typedef struct
{
    int func;
    char *t;
    Value value;
} Plot;

typedef struct
{
    int size;
    Plot **p;
} MultiPlot;

MultiPlot *mp;
void initialize()
{
    mp = calloc(1, sizeof(MultiPlot));
    mp->size = -1;
    mp->p = calloc(10, sizeof(Plot *));
}

void addPlot(Value v)
{
    Plot *p = calloc(1, sizeof(Plot));
    p->func = 1;
    p->t = "rabin";
    p->value = v;
    mp->size++;
    mp->p[mp->size] = p;
    redraw_drawing_area(areaGlobal);
}

static void start_main_draw(Grid *grid, int W, int h)
{
    for (int j = 0; j <= mp->size; j++)
    {
        g_print("lol:%d", mp->size);
        double px = -1.0 * W;
        for (double i = -1.0 * W / gap; i <= W / gap; i += 0.01)
        {
            // point(grid, i, f(i));
            int indexT = (int)j;
            double py = evaluateAST(&mp->p[indexT]->value.ast, px);
            double y = evaluateAST(&mp->p[indexT]->value.ast, i);

            createLine(grid, px, py, i, y);
            px = i;
        }
        // if (j % 2 == 0)
        // {

        // }
        // else
        // {

        //     double px = -1.0 * W;
        //     for (double i = -1.0 * W / gap; i <= W / gap; i += 0.01)
        //     {
        //         createLine(grid, px, f2(px), i, f2(i));
        //         px = i;
        //     }
        // }
    }
}

static void draw_function(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data)
{
    GtkWidget *gGrid = GTK_WIDGET(user_data);
    cairo_set_source_rgb(cr, 1, 1, 1);

    cairo_paint(cr);
    Grid *grid = init_grid(cr, gap);
    create_grid(grid, width, height);

    start_main_draw(grid, width, height);
}
static void redraw_drawing_area(gpointer user_data)
{
    gtk_widget_queue_draw(GTK_WIDGET(user_data));
    g_print("gap=%d\n", gap);
}
static void ZoomIn(GtkWidget *widget, gpointer user_data)
{
    gap += 10;
    redraw_drawing_area(user_data);
}
static void ZoomOut(GtkWidget *widget, gpointer user_data)
{
    gap -= 10;
    if (gap <= 0)
    {
        gap = 10;
    }
    redraw_drawing_area(user_data);
}

GtkWidget *one_area_input(GtkWidget *area, char *v)
{
    GtkWidget *button = gtk_button_new_with_label("+");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *label1 = gtk_label_new(v);
    gtk_box_append(GTK_BOX(box), label1);
    gtk_widget_set_hexpand(label1, TRUE);

    gtk_box_append(GTK_BOX(box), button);
    return box;
}
static void add_one_area_input(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *listbox = GTK_WIDGET(user_data);
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(eqnEntry));
    const char *codeEqn = gtk_entry_buffer_get_text(buffer);

    char *mutableCodeEqn = g_strdup(codeEqn);

    Value v = func(mutableCodeEqn, 1);
    // g_print("Value: %s\n", v);

    // if (!v.error.isError)
    // {
    GtkWidget *area = one_area_input(areaGlobal, mutableCodeEqn);
    gtk_list_box_append(GTK_LIST_BOX(listbox), area);
    addPlot(v);
    // }
    // g_free(mutableCodeEqn);
}

static void design_box(GtkBox *box, GtkWidget *area)
{
    initialize();
    // GtkWidget *label = gtk_label_new("Write function here");
    eqnEntry = gtk_entry_new();
    GtkWidget *topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *headerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *addAreaButton = gtk_button_new_with_label("add");
    GtkWidget *listbox = gtk_list_box_new();
    areaGlobal = area;

    gtk_widget_set_hexpand(eqnEntry, TRUE);
    // gtk_widget_set_halign(eqnEntry, GTK_ALIGN_CENTER);

    // GtkWidget *first_area = one_area_input(area);
    // GtkWidget *second_area = one_area_input(area);

    gtk_box_append(GTK_BOX(headerBox), eqnEntry);
    gtk_box_append(GTK_BOX(headerBox), addAreaButton);

    gtk_box_append(GTK_BOX(topBox), headerBox);
    gtk_box_append(GTK_BOX(box), topBox);
    // gtk_widget_set_hexpand(first_area, TRUE);
    // gtk_widget_set_hexpand(second_area, TRUE);
    // gtk_list_box_append(GTK_LIST_BOX(listbox), first_area);
    gtk_box_append(GTK_BOX(box), listbox);
    g_signal_connect(addAreaButton, "clicked", G_CALLBACK(add_one_area_input), listbox);

    // gtk_box_append(GTK_BOX(box), first_area);
    // gtk_box_append(GTK_BOX(box), second_area);
}

static void app_activate(GApplication *app, gpointer user_data)
{
    GtkWidget *win = gtk_application_window_new(GTK_APPLICATION(app));
    GtkWidget *area = gtk_drawing_area_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *grid = gtk_grid_new();

    gtk_window_set_title(GTK_WINDOW(win), "GRAPH_BUILDER");
    gtk_window_set_default_size(GTK_WINDOW(win), 1669, 1000);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_function, grid, NULL);

    gtk_grid_attach(GTK_GRID(grid), box, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), area, 1, 0, 3, 1);
    design_box(GTK_BOX(box), area);

    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_hexpand(area, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    gtk_widget_set_vexpand(area, TRUE);

    GtkWidget *fixed = gtk_fixed_new();
    GtkWidget *zoomInButton = gtk_button_new_with_label("+");
    GtkWidget *zoomOutButton = gtk_button_new_with_label("-");

    g_signal_connect(zoomInButton, "clicked", G_CALLBACK(ZoomIn), area);
    g_signal_connect(zoomOutButton, "clicked", G_CALLBACK(ZoomOut), area);

    const char *class[] = {"zoom", NULL};
    gtk_widget_set_css_classes(zoomInButton, class);
    gtk_widget_set_css_classes(zoomOutButton, class);

    gtk_fixed_put(GTK_FIXED(fixed), zoomInButton, 10, 10);
    gtk_fixed_put(GTK_FIXED(fixed), zoomOutButton, 10, 100);

    gtk_grid_attach(GTK_GRID(grid), fixed, 1, 0, 3, 1);
    gtk_widget_set_size_request(zoomInButton, 60, 60);
    gtk_widget_set_size_request(zoomOutButton, 60, 60);

    gtk_window_set_child(GTK_WINDOW(win), grid);

    applyCSS(GTK_WINDOW(win));

    gtk_window_present(GTK_WINDOW(win));
}

#define APPLICATION_ID "com.github.EV-OD"

int main(int argc, char **argv)
{
    GtkApplication *app;
    int stat;

    app = gtk_application_new(APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
    stat = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return stat;
}
