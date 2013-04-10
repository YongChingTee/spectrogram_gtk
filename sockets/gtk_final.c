#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "fft_socket_header.h"

#define AXIS_START 21
#define CAMERA_WIDTH 1024

GtkWidget *image;
char buff[20];
float *buffer;
int samp_rate;
int CAMERA_HEIGHT;
int port_num;

struct fft_header header;

struct pixel
{
	unsigned char red, green, blue;
};

struct pixel* rgbImage;

void error(const char *msg)
{
    perror(msg);
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
	int sockfd, newsockfd;
    socklen_t clilen;
    // char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
	/*Get new data from socket*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_addr.sin_port = htons(port_num);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
           error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
       error("ERROR on accept");
       
    n = read(newsockfd, &header, sizeof(struct fft_header));
    if (n < 0)
        error("ERROR reading from socket");

    printf("header.constSync is %X\n", header.constSync);
        // printf("header_len is %d\n", sizeof(struct fft_header));

    if(header.constSync != 0xACFDFFBC)
            error("ERROR reading from socket, incorrect header placement\n");

	n = read(newsockfd, (char *) buffer, samp_rate * sizeof(float));
   	if (n < 0)
        error("ERROR reading from socket");
	close(newsockfd);
    close(sockfd);
	/*END*/

	/*shifting data in pixbuff*/
	for(j=0; j<CAMERA_HEIGHT-AXIS_START; j++)
	{
		for(i = CAMERA_WIDTH-1; i >=AXIS_START+2; i--)
		{
			rgbImage[j*CAMERA_WIDTH+i] = rgbImage[j*CAMERA_WIDTH+i-1];
		}
	}
	
	for(i = CAMERA_HEIGHT-AXIS_START-1; i>=0 ; i--)
	{		
		rgbImage[i*CAMERA_WIDTH+AXIS_START+1].blue = MIN((MAX((4*(0.75-buffer[i])), 0.)), 1.) * 255;
		rgbImage[i*CAMERA_WIDTH+AXIS_START+1].red = MIN((MAX((4*(buffer[i]-0.25)), 0.)), 1.) * 255;
		rgbImage[i*CAMERA_WIDTH+AXIS_START+1].green = MIN((MAX((4*fabs(buffer[i]-0.5)-1.), 0.)), 1.) * 255;
	}
	//count++;
	
	
	loadImage();
	return 1;
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
	int i, j, k;
	int sockfd, newsockfd;
    socklen_t clilen;
	int n;
    // char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
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

    if (argc < 2)
    {
     fprintf(stderr,"ERROR, no port provided\n");
     exit(1);
    }
	/*Initiate header*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    port_num = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_addr.sin_port = htons(port_num);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
       error("ERROR on accept");
       
    n = read(newsockfd, &header, sizeof(struct fft_header));
    if (n < 0)
       error("ERROR reading from socket");

    printf("header.constSync is %X\n", header.constSync);
        // printf("header_len is %d\n", sizeof(struct fft_header));

    if(header.constSync != 0xACFDFFBC)
        error("ERROR reading from socket, incorrect header placement\n");
	close(newsockfd);
    close(sockfd);
	/*End*/
	
	samp_rate = header.ptsPerFFT;
	CAMERA_HEIGHT = samp_rate + AXIS_START;
	rgbImage = malloc(sizeof(struct pixel) * (CAMERA_HEIGHT+CAMERA_WIDTH));
	buffer = malloc(sizeof(float) * samp_rate);

	//gtk initialization
	
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
	/*Initialize screen with color*/
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
	
	//call shift every 1msec
	gint func_ref = g_timeout_add(1, shift, NULL);
	
	gtk_main();
	g_source_remove (func_ref);
   
    return 0;

}
