#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "text.h"

#define CAMERA_WIDTH 512
#define CAMERA_HEIGHT 512
unsigned char rgbImage[CAMERA_WIDTH*3*CAMERA_HEIGHT];
GtkWidget *image;
int loadImage(unsigned char *data)
{
//printf("Got image!\n");
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(data, GDK_COLORSPACE_RGB, FALSE, 8, CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_WIDTH  * 3, NULL, NULL);
	gtk_image_set_from_pixbuf((GtkImage*) image, pixbuf);
	gtk_widget_queue_draw(image);
//printf("Loaded\n");

return 0;
}

int shift()
{
	int i, j;
	
	for(j=0; j<(CAMERA_HEIGHT-21); j++)
	{
		for(i = CAMERA_WIDTH*3-1; i >=68; i-=3)
		{
			rgbImage[j*CAMERA_WIDTH*3+i] = rgbImage[j*CAMERA_WIDTH*3+i-3];
			rgbImage[j*CAMERA_WIDTH*3+i-1] = rgbImage[j*CAMERA_WIDTH*3+i-4];
			rgbImage[j*CAMERA_WIDTH*3+i-2] = rgbImage[j*CAMERA_WIDTH*3+i-5];
		}
	}
	
	for(i = 66; i< CAMERA_WIDTH*(CAMERA_HEIGHT-21)*3; i+=(CAMERA_WIDTH*3))
	{		
		rgbImage[i] = i%255;
		rgbImage[i+1] = i%255;
		rgbImage[i+2] = i%255;
	}
	
	loadImage(rgbImage);
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
	/*
	for(i = 1; i< CAMERA_WIDTH*CAMERA_HEIGHT*3; i+=3)
	{		
		rgbImage[i] = i%255;
		//rgbImage[i+1] = 255;
		//rgbImage[i+2] = 255;
	}
	/*
	for(i = 2; i< CAMERA_WIDTH*CAMERA_HEIGHT*3; i+=3)
	{		
		rgbImage[i] = 255;
		//rgbImage[i+1] = 255;
		//rgbImage[i+2] = 255;
	}
	*/
	for(i = 63; i< CAMERA_WIDTH*CAMERA_HEIGHT*3; i+=(CAMERA_WIDTH*3))
	{		
		rgbImage[i] = 255;
		rgbImage[i+1] = 255;
		rgbImage[i+2] = 255;
	}

	for(i = 0; i< CAMERA_WIDTH*3; i+=3)
	{		
		rgbImage[i+(CAMERA_HEIGHT-21)*CAMERA_WIDTH*3] = 255;
		rgbImage[i+1+(CAMERA_HEIGHT-21)*CAMERA_WIDTH*3] = 255;
		rgbImage[i+2+(CAMERA_HEIGHT-21)*CAMERA_WIDTH*3] = 255;
	}
	for(i = 99; i< CAMERA_WIDTH*(CAMERA_HEIGHT-21)*3; i+=(CAMERA_WIDTH*3))
	{		
		rgbImage[i] = 255;
		rgbImage[i+1] = 255;
		rgbImage[i+2] = 255;
	}
	for(i = 102; i< CAMERA_WIDTH*(CAMERA_HEIGHT-21)*3; i+=(CAMERA_WIDTH*3))
	{		
		rgbImage[i] = 255;
		rgbImage[i+1] = 255;
		rgbImage[i+2] = 255;
	}
	
	loadImage(rgbImage);
	gint func_ref = g_timeout_add(100, shift, NULL);
	gtk_main();
	g_source_remove (func_ref);
	
/*
	while(1)
	{
		image = gtk_image_new();
		for(j=0; j<CAMERA_HEIGHT; j++)
		{
			for(i = CAMERA_WIDTH; i >=0; i--)
			{
				rgbImage[j*CAMERA_WIDTH+(i)*3] = rgbImage[j*CAMERA_WIDTH+(i-1)*3];
				rgbImage[j*CAMERA_WIDTH+(i)*3+1] = rgbImage[j*CAMERA_WIDTH+(i-1)*3+1];
				rgbImage[j*CAMERA_WIDTH+(i)*3+2] = rgbImage[j*CAMERA_WIDTH+(i-1)*3+2];
			}
		}
		loadImage(rgbImage);
		usleep(1000000);
	};
*/
	gtk_main();
   
    return 0;

}
