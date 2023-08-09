#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "grid.c"
#include "compiler.c"

static void redraw_drawing_area(gpointer user_data);

GtkWidget *areaGlobal;
GtkWidget *eqnEntry;
GtkWidget *listbox;

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
    int show;
    double r;
    double g;
    double b;
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

void remove_plot_from_multiplot(int index)
{
    if (mp == NULL || mp->size < 0 || mp->p == NULL || index < 0 || index > mp->size)
    {
        return;
    }

    free(mp->p[index]);

    for (int i = index; i < mp->size; i++)
    {
        mp->p[i] = mp->p[i + 1];
    }

    mp->size--;
}

void addPlot(Value v)
{
    double r, g, b;
    randomize_color(&r, &g, &b);
    Plot *p = calloc(1, sizeof(Plot));
    p->func = 1;
    p->t = "rabin";
    p->show = 1;
    p->value = v;
    p->r = r;
    p->g = g;
    p->b = b;
    mp->size++;
    mp->p[mp->size] = p;
    redraw_drawing_area(areaGlobal);
}

static void start_main_draw(Grid *grid, int W, int h)
{
    for (int j = 0; j <= mp->size; j++)
    {
        int indexT = (int)j;
        Plot *p1 = mp->p[indexT];
        if (mp->p[indexT]->show)
        {
            double px = -1.0 * W;
            for (double i = -1.0 * W / gap; i <= W / gap; i += 0.001)
            {
                double py = evaluateAST(&mp->p[indexT]->value.ast, px);
                double y = evaluateAST(&mp->p[indexT]->value.ast, i);
                cairo_set_source_rgb(grid->cr, p1->r, p1->g, p1->b);
                // point(grid, i, y);
                // this needs fix
                createLine(grid, px, py, i, y);
                px = i;
            }
        }
    }
}
void removeWidgetFromList(GtkButton *button, gpointer data)
{
    GtkWidget *box = GTK_WIDGET(data);
    GtkListBoxRow *row = GTK_LIST_BOX_ROW(gtk_widget_get_ancestor(box, GTK_TYPE_LIST_BOX_ROW));
    g_print("\ntest0\n");
    if (row != NULL)
    {
        g_print("\ntest0\n");

        int index = gtk_list_box_row_get_index(row);
        g_print("%d", index);
        remove_plot_from_multiplot(index);
        g_print("index:%d\n", index);
        // gtk_widget_unparent(box);
        g_print("\ntest1\n");
        gtk_list_box_remove(GTK_LIST_BOX(listbox), GTK_WIDGET(row));
        g_print("\ntest2\n");
        redraw_drawing_area(areaGlobal);
        g_print("\ntest3\n");
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
    GtkWidget *button = gtk_button_new_with_label("🗑");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *label1 = gtk_label_new(v);
    const char *class1[] = {"eqnlabel", NULL};
    gtk_widget_set_css_classes(label1, class1);

    const char *class2[] = {"deleteBtn", NULL};
    gtk_widget_set_css_classes(button, class2);
    gtk_box_append(GTK_BOX(box), label1);
    gtk_widget_set_hexpand(label1, TRUE);

    gtk_box_append(GTK_BOX(box), button);
    g_signal_connect(button, "clicked", G_CALLBACK(removeWidgetFromList), box);
    return box;
}

static void add_one_area_input(GtkWidget *widget, gpointer user_data)
{
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(eqnEntry));
    const char *codeEqn = gtk_entry_buffer_get_text(buffer);

    char *mutableCodeEqn = g_strdup(codeEqn);

    Value v = func(mutableCodeEqn, 1);

    GtkWidget *area = one_area_input(areaGlobal, mutableCodeEqn);
    gtk_list_box_append(GTK_LIST_BOX(listbox), area);
    addPlot(v);
}

static void design_box(GtkBox *box, GtkWidget *area)
{
    initialize();
    eqnEntry = gtk_entry_new();
    GtkWidget *topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *headerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *addAreaButton = gtk_button_new_with_label("add");
    const char *class2[] = {"addBtn", NULL};
    gtk_widget_set_css_classes(addAreaButton, class2);
    listbox = gtk_list_box_new();
    areaGlobal = area;
    const char *class1[] = {"listbox", NULL};
    gtk_widget_set_css_classes(listbox, class1);

    gtk_widget_set_hexpand(eqnEntry, TRUE);
    gtk_widget_set_vexpand(listbox, TRUE);

    gtk_box_append(GTK_BOX(headerBox), eqnEntry);
    gtk_box_append(GTK_BOX(headerBox), addAreaButton);

    gtk_box_append(GTK_BOX(topBox), headerBox);
    gtk_box_append(GTK_BOX(box), topBox);
    gtk_box_append(GTK_BOX(box), listbox);
    g_signal_connect(addAreaButton, "clicked", G_CALLBACK(add_one_area_input), listbox);
    const char *class[] = {"sidebar", NULL};
    gtk_widget_set_css_classes(GTK_WIDGET(box), class);
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
