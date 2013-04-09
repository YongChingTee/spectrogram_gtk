#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#define CAMERA_WIDTH 1024
#define CAMERA_HEIGHT 600
#define AXIS_START 21

struct pixel
{
	unsigned char red, green, blue;
} rgbImage[CAMERA_WIDTH * CAMERA_HEIGHT];

GtkWidget *image;
int count = 0;

int loadImage()
{
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data((unsigned char*)rgbImage, GDK_COLORSPACE_RGB, FALSE, 8, CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_WIDTH  * 3, NULL, NULL);
	gtk_image_set_from_pixbuf((GtkImage*) image, pixbuf);
	gtk_widget_queue_draw(image);

return 0;
}

int shift()
{
	int i, j;
	
	for(j=0; j<CAMERA_HEIGHT-AXIS_START; j++)
	{
		for(i = CAMERA_WIDTH-1; i >=AXIS_START+2; i--)
		{
			rgbImage[j*CAMERA_WIDTH+i] = rgbImage[j*CAMERA_WIDTH+i-1];
		}
	}
	
	for(i = 0; i< CAMERA_HEIGHT-AXIS_START; i++)
	{		
		rgbImage[i*CAMERA_WIDTH+AXIS_START+1].red = (i + count)%255;
		rgbImage[i*CAMERA_WIDTH+AXIS_START+1].green = (i + count)%255;
		rgbImage[i*CAMERA_WIDTH+AXIS_START+1].blue = (i + count)%255;
	}
	count++;
	
	loadImage();
	return 1;
}

int main( int argc, char *argv[])
{
    GtkWidget *window;
	int i, j, k;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Image2");
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 2);
    image = gtk_image_new();
    gtk_container_add(GTK_CONTAINER(window), image);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    gtk_widget_show_all(window);
	
	
	//AXIS
	for(i = 0; i< CAMERA_HEIGHT; i++)
	{		
		rgbImage[i*CAMERA_WIDTH + AXIS_START].red = 255;
		rgbImage[i*CAMERA_WIDTH + AXIS_START].green = 255;
		rgbImage[i*CAMERA_WIDTH + AXIS_START].blue = 255;
	}

	for(i = 0; i< CAMERA_WIDTH; i++)
	{		
		rgbImage[i+(CAMERA_HEIGHT-AXIS_START)*CAMERA_WIDTH].red = 255;
		rgbImage[i+(CAMERA_HEIGHT-AXIS_START)*CAMERA_WIDTH].green = 255;
		rgbImage[i+(CAMERA_HEIGHT-AXIS_START)*CAMERA_WIDTH].blue = 255;
	}
	//first line
	for(i = 0; i< CAMERA_HEIGHT-AXIS_START; i++)
	{		
		rgbImage[i*CAMERA_WIDTH + 27].red = 255;
		rgbImage[i*CAMERA_WIDTH + 27].green = 255;
		rgbImage[i*CAMERA_WIDTH + 27].blue = 255;
	}
	for(i = 0; i< CAMERA_HEIGHT-AXIS_START; i++)
	{		
		rgbImage[i*CAMERA_WIDTH + 28].red = 255;
		rgbImage[i*CAMERA_WIDTH + 28].green = 255;
		rgbImage[i*CAMERA_WIDTH + 28].blue = 255;
	}
	
	loadImage();
	gint func_ref = g_timeout_add(5, shift, NULL);
	gtk_main();
	g_source_remove (func_ref);

	gtk_main();
   
    return 0;

}
