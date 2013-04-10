#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>

#define AXIS_START 0
#define SAMP_RATE 256
#define CAMERA_WIDTH 1024
#define CAMERA_HEIGHT (SAMP_RATE+AXIS_START)

struct pixel
{
	unsigned char red, green, blue;
};

GtkWidget *image;
FILE *file;
//int count = 0;
char buff[20];
float buffer[SAMP_RATE];
struct pixel *rgbImage;



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
		for(i = CAMERA_WIDTH-1; i >=AXIS_START+1; i--)
		{
			rgbImage[j*CAMERA_WIDTH+i] = rgbImage[j*CAMERA_WIDTH+i-1];
		}
	}
	
	//get data from file
	for(i = 0 ; i < SAMP_RATE; i++)
	{
			if(feof(file))
				buffer[i] = 0;
			else
				fscanf(file, "%f", &buffer[i]);
	}
	//fclose(file);
	
	for(i = CAMERA_HEIGHT-AXIS_START-1; i>=0 ; i--)
	{		
		rgbImage[i*CAMERA_WIDTH+AXIS_START].blue = MIN((MAX((4*(0.75-buffer[i])), 0.)), 1.) * 255;
		rgbImage[i*CAMERA_WIDTH+AXIS_START].red = MIN((MAX((4*(buffer[i]-0.25)), 0.)), 1.) * 255;
		rgbImage[i*CAMERA_WIDTH+AXIS_START].green = MIN((MAX((4*fabs(buffer[i]-0.5)-1.), 0.)), 1.) * 255;
	}
	//count++;
	
	
	loadImage();
	return 1;
}

int main( int argc, char *argv[])
{
    GtkWidget *window;
	int i, j, k;
	rgbImage = malloc(sizeof(struct pixel) * (CAMERA_WIDTH*CAMERA_HEIGHT));

	file = fopen("test_files/chirpFFT.txt", "r");	//open file

	if(file == NULL)
	{
		printf("ERROR! no file");
		system("pause");
	}

	//gtk initialization
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Spectrogram");
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 2);
    image = gtk_image_new();
    gtk_container_add(GTK_CONTAINER(window), image);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    gtk_widget_show_all(window);
	
	
	//AXIS
	/*
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
	*/

	for(j = 0; j < CAMERA_WIDTH-AXIS_START; j++)
	{
		for(i = CAMERA_HEIGHT-AXIS_START-1; i>=0 ; i--)
		{		
			rgbImage[j+i*CAMERA_WIDTH+AXIS_START+1].blue = MIN((MAX((4*(0.75-0)), 0.)), 1.) * 255;
			rgbImage[j+i*CAMERA_WIDTH+AXIS_START+1].red = MIN((MAX((4*(0-0.25)), 0.)), 1.) * 255;
			rgbImage[j+i*CAMERA_WIDTH+AXIS_START+1].green = MIN((MAX((4*fabs(0-0.5)-1.), 0.)), 1.) * 255;
		}
	}
	
	//loadeImage function
	loadImage();
	
	//call shift every 10msec
	gint func_ref = g_timeout_add(10, shift, NULL);
	
	gtk_main();
	g_source_remove (func_ref);
   
    return 0;

}
