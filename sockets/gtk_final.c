#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "fft_socket_header.h"

//#define AXIS_START 0
#define CAMERA_WIDTH 1024
#define BLUEMAC(x) (MIN((MAX((4*(0.75-(x))), 0.)), 1.) * 255)
#define REDMAC(x) (MIN((MAX((4*((x)-0.25)), 0.)), 1.) * 255)
#define GREENMAC(x) (MIN((MAX((4*fabs((x)-0.5)-1.), 0.)), 1.) * 255)


GtkWidget *image;
char buff[20];
float *buffer;
int samp_rate;
int CAMERA_HEIGHT;
int port_num;
int sockfd, newsockfd;
socklen_t clilen;
int n;
struct sockaddr_in serv_addr, cli_addr;

struct fft_header header;

struct pixel
{
	unsigned char red, green, blue;
};

struct pixel* rgbImage;


void error1(const char *msg)
{
    fprintf(stderr, "%s: %s \n", msg, strerror(errno));
    exit(1);
}

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
	//newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	//if(1)
	fprintf(stderr, "Reading header... ");
	n = read(newsockfd, &header, sizeof(struct fft_header));
	if (n < 0)
	    error1("ERROR reading from socket");
	else if (n > 0)
	{
		printf("header.constSync is %X\n", header.constSync);
		// printf("header_len is %d\n", sizeof(struct fft_header));
		if(header.constSync != 0xACFDFFBC)
			error1("ERROR reading from socket, incorrect header placement\n");
	}
	else if( n == 0)
	{
		printf("Sender has closed connection\n");
		exit(0);
	}

	fprintf(stderr, "Reading data... ");
	n = read(newsockfd, (char *) buffer, header.ptsPerFFT * sizeof(float));
	if (n < 0)
	    error1("ERROR reading from socket");
	else if( n == 0)
	{
		printf("Sender has closed connection\n");
		exit(0);
	}

	/*END*/

	/*shifting data in pixbuff*/
	for(j=0; j<CAMERA_HEIGHT; j++)
	{
		for(i = CAMERA_WIDTH-1; i >=1; i--)
		{
			rgbImage[j*CAMERA_WIDTH+i] = rgbImage[j*CAMERA_WIDTH+i-1];
		}
	}
	
	for(i = CAMERA_HEIGHT-1; i>=0 ; i--)
	{		
		rgbImage[i*CAMERA_WIDTH].blue = BLUEMAC(buffer[i]);
		rgbImage[i*CAMERA_WIDTH].red = REDMAC(buffer[i]);
		rgbImage[i*CAMERA_WIDTH].green = GREENMAC(buffer[i]);
	}
	//count++;
	
	
	loadImage();
	return 1;
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
	int i, j, k;
	/*initiate gtk*/
	gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Spectrogram");
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 2);
    image = gtk_image_new();
    gtk_container_add(GTK_CONTAINER(window), image);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    gtk_widget_show_all(window);
	/*END*/
	/*Initiate header*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error1("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    port_num = 51717;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_addr.sin_port = htons(port_num);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error1("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
	
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
       error1("ERROR on accept");
	//First Header
    fprintf(stderr, "Reading header... ");  
    n = read(newsockfd, &header, sizeof(struct fft_header));
    if (n < 0)
       error1("ERROR reading from socket");
    else if (n > 0)
	{
		printf("header.constSync is %X\n", header.constSync);
    	// printf("header_len is %d\n", sizeof(struct fft_header));
		if(header.constSync != 0xACFDFFBC)
			error1("ERROR reading from socket, incorrect header placement\n");
	}
	else if( n == 0)
	{
		printf("Sender has closed connection\n");
		exit(0);
	}

	//Initializing structures
	samp_rate = header.ptsPerFFT;
	CAMERA_HEIGHT = samp_rate;
	rgbImage = malloc(sizeof(struct pixel) * (CAMERA_HEIGHT*CAMERA_WIDTH));
	buffer = malloc(sizeof(float) * samp_rate);

	//First Data
	fprintf(stderr, "Reading data... ");
    n = read(newsockfd, (char *) buffer, header.ptsPerFFT * sizeof(float));
    if (n < 0)
    	error1("ERROR reading from socket");
	else if( n == 0)
	{
		printf("Sender has closed connection\n");
		exit(0);
	}
	
	/*End*/
	
	//gtk initialization
	/*Initialize screen with color*/
	for(j = 0; j < CAMERA_WIDTH; j++)
	{
		for(i = CAMERA_HEIGHT-1; i>=0 ; i--)
		{		
			rgbImage[j+i*CAMERA_WIDTH].blue = BLUEMAC(0);
			rgbImage[j+i*CAMERA_WIDTH].red = REDMAC(0);
			rgbImage[j+i*CAMERA_WIDTH].green = GREENMAC(0);
		}
	}
	for(i = CAMERA_HEIGHT-1; i>=0 ; i--)
	{		
		rgbImage[i*CAMERA_WIDTH].blue = BLUEMAC(buffer[i]);
		rgbImage[i*CAMERA_WIDTH].red = REDMAC(buffer[i]);
		rgbImage[i*CAMERA_WIDTH].green = GREENMAC(buffer[i]);
	}
	
	//loadeImage function
	loadImage();
	
	//call shift every 1msec
	gint func_ref = g_timeout_add(10, shift, NULL);
	
	gtk_main();
	g_source_remove (func_ref);
   
    return 0;

}
