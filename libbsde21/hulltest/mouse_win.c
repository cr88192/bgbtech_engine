#include <windows.h>

int	mouse_lx, mouse_ly, mouse_lb;
POINT	mouse_pos;

extern int window_center_x, window_center_y;
extern int window_mouse_buttons;


int Mouse_GetRelPos(int *dx, int *dy, int *mb)
{
	int x, y, b;

	GetCursorPos (&mouse_pos);
//	SetCursorPos (window_center_x, window_center_y);
	x=mouse_pos.x-window_center_x;
	y=mouse_pos.y-window_center_y;

	b=window_mouse_buttons;

	if ((x<=-400) || (x>=400) || (y<=-300) || (y>=300))
	{
		x=mouse_lx;
		y=mouse_ly;
//		b=mouse_lb;
	}

	*dx=x-mouse_lx;
	*dy=y-mouse_ly;
	mouse_lx=x;
	mouse_ly=y;
	mouse_lb=b;

	*mb=b&7;

	return(0);
}

int Mouse_GetPos(int *mx, int *my, int *mb)
{
	int x, y, b;

	GetCursorPos (&mouse_pos);
//	SetCursorPos (window_center_x, window_center_y);
	x=mouse_pos.x-window_center_x;
	y=mouse_pos.y-window_center_y;

	b=window_mouse_buttons;

	if ((x<=-400) || (x>=400) || (y<=-300) || (y>=300))
	{
		x=mouse_lx;
		y=mouse_ly;
//		b=mouse_lb;
	}

	*mx=x;
	*my=y;
	mouse_lx=x;
	mouse_ly=y;
	mouse_lb=b;

	*mb=b&7;

	return(0);
}

int Mouse_SetPos(int mx, int my)
{
	int x, y, b;

	x=window_center_x+mx;
	y=window_center_y+my;

	SetCursorPos (x, y);
	mouse_lx = mx;
	mouse_ly = my;

	return(0);
}
