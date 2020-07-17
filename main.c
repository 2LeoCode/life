#define _USE_MATH_DEFINES
#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define WIN_X 800
#define WIN_Y 600
#define SQ(X) X * X
#define C_RED 0
#define C_GRN 1
#define C_BLU 2
#define BALL_SIZE 15

typedef struct		s_ball
{
	int				color;
	double			dir_x;
	double			dir_y;
	int				pos_x;
	int				pos_y;
	struct s_ball	*next;
}					t_ball;

t_ball	*ball_new(int color)
{
	t_ball	*tmp;

	if ((color - C_RED) && (color - C_GRN) && (color - C_BLU))
	{
		printf("Error, wrong color\n");
		return (NULL);
	}
	if (!(tmp = malloc(sizeof(t_ball))))
		return (NULL);
	tmp->color = color;
	tmp->pos_x = rand() % (WIN_X - BALL_SIZE - BALL_SIZE + 1) + BALL_SIZE;
	tmp->pos_y = rand() % (WIN_Y - BALL_SIZE - BALL_SIZE + 1) + BALL_SIZE;
	tmp->dir_x = ((float)rand() / RAND_MAX) * 2 - 1;
	tmp->dir_y = sqrt(1 - (SQ(tmp->dir_x)));
	tmp->next = NULL;
	return (tmp);
}

t_ball	*ball_add_front(t_ball *ball, int color)
{
	t_ball	*tmp;

	if (!(tmp = ball_new(color)))
		return (NULL);
	tmp->next = ball;
	return (tmp);
}

void	draw_circle(SDL_Renderer *ren, int x, int y, double alpha)
{
	double	alpha_tot;
	double	ix;
	double	iy;
	int		size;

	size = BALL_SIZE;
	//while (size >= 1)
	//{
		alpha_tot = 0;
		ix = x + size;
		iy = y;
		while (alpha_tot < 360)
		{
			SDL_RenderDrawPoint(ren, ix, iy);
			ix = size * cos(alpha_tot * M_PI / 180) + x;
			iy = size * sin(alpha_tot * M_PI / 180) + y;
			alpha_tot += alpha;
		}
		size--;
	//}
}

void	update_pos(t_ball **ball, double alpha)
{
	t_ball	*head;
	t_ball	*cur;
	double	alpha_tot;
	double	tanx;
	double	tany;

	cur = *ball;
	while (cur)
	{
		if (((WIN_X - cur->pos_x <= BALL_SIZE) && (cur->dir_x > 0))
		|| ((cur->pos_x <= BALL_SIZE) && (cur->dir_x < 0)))
			cur->dir_x = -cur->dir_x;
		if (((WIN_Y - cur->pos_y <= BALL_SIZE) && (cur->dir_y > 0))
		|| ((cur->pos_y <= BALL_SIZE) && (cur->dir_y < 0)))
			cur->dir_y = -cur->dir_y;
		cur->pos_x += cur->dir_x * 5;
		cur->pos_y += cur->dir_y * 5;
		cur = cur->next;
	}
	alpha_tot = 0;
	cur = *ball;
	while (cur)
	{
		head = cur->next;
		while (head)
		{
			if (sqrt(SQ(head->pos_x - cur->pos_x) + SQ(head->pos_y - cur->pos_y))
			<= (2 * BALL_SIZE))
			{
				tanx = cur->pos_x + BALL_SIZE;
				tany = cur->pos_y;
				while (sqrt(SQ(head->pos_x - tanx) + SQ(head->pos_y - tany)
				<= BALL_SIZE))
				{
					tanx = BALL_SIZE * cos(alpha_tot * M_PI / 180) + cur->pos_x;
					tany = BALL_SIZE * sin(alpha_tot * M_PI / 180) + cur->pos_y;
					alpha_tot += alpha;
				}
				tanx = BALL_SIZE * cos((alpha_tot - alpha) + 90 * M_PI / 180) + cur->pos_x;
				tany = BALL_SIZE * sin((alpha_tot - alpha) + 90 * M_PI / 180) + cur->pos_y;
				head->dir_x = cos(atan(head->dir_y / head->dir_x)
				+ acos((tanx * head->dir_x + tany * head->dir_y)
				/ (sqrt(SQ(tanx) + SQ(tany)) * sqrt(SQ(head->dir_x) + SQ(head->dir_y)))));
				head->dir_y = sqrt(1 - SQ(head->dir_x));
				cur->dir_x = cos(atan(cur->dir_y / cur->dir_x)
				+ acos((tanx * cur->dir_x + tany * cur->dir_y)
				/ (sqrt(SQ(tanx) + SQ(tany)) * sqrt(SQ(cur->dir_x) + SQ(cur->dir_y)))));
				cur->dir_y = sqrt(1 - SQ(cur->dir_x));
			}
			head = head->next;
		}
		cur = cur->next;
	}
}

void	display_balls(t_ball *ball, SDL_Renderer *ren, double alpha)
{
	while (ball)
	{
		if (ball->color == C_RED)
			SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
		if (ball->color == C_GRN)
			SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
		if (ball->color == C_BLU)
			SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
		draw_circle(ren, ball->pos_x, ball->pos_y, alpha);
		ball = ball->next;
	}
}

void	check_start_pos(t_ball ** balls)
{
	t_ball *head;
	t_ball *cur;

	cur = *balls;
	while (cur)
	{
		head = cur->next;
		while (head)
		{
			while (sqrt(SQ(head->pos_x - cur->pos_x) + SQ(head->pos_y - cur->pos_y))
			<= (2 * BALL_SIZE))
			{
				head->pos_x = rand() % (WIN_X - BALL_SIZE - BALL_SIZE + 1) + BALL_SIZE;
				head->pos_y = rand() % (WIN_Y - BALL_SIZE - BALL_SIZE + 1) + BALL_SIZE;
			}
			head = head->next;
		}
		cur = cur->next;
	}
}

int		main(void)
{
	SDL_Window		*win;
	SDL_Renderer	*ren;
	t_ball			*ball_list;
	clock_t			time_var;
	double 			alpha;

	alpha = 1;
	while (BALL_SIZE * sin(alpha) > 1)
		alpha /= 1.1;
	srand(time(NULL));
	if (!(ball_list = ball_new(C_RED)))
		return (-1);
	ball_list = ball_add_front(ball_list, C_GRN);
	ball_list = ball_add_front(ball_list, C_BLU);
	ball_list = ball_add_front(ball_list, C_RED);
	ball_list = ball_add_front(ball_list, C_GRN);
	ball_list = ball_add_front(ball_list, C_BLU);
	check_start_pos(&ball_list);
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("error\n");
		return (-1);
	}
	win = SDL_CreateWindow("Test", 0, 0, WIN_X, WIN_Y, 0);
	ren = SDL_CreateRenderer(win, -1, 0);
	SDL_bool isquit = SDL_FALSE;
	SDL_Event event;
	while (!isquit)
	{
		display_balls(ball_list, ren, alpha);
		SDL_RenderPresent(ren);
		time_var = clock();
		while (clock() < time_var + 10)
			continue;
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderClear(ren);
		update_pos(&ball_list, alpha);
    	if (SDL_PollEvent(&event) && (event.type == SDL_QUIT))
            isquit = SDL_TRUE;
	}
	SDL_DestroyWindow(win);
	return (0);
}