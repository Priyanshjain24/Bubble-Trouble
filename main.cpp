#include <simplecpp>
#include "shooter.h"
#include "bubble.h"
#include <sstream>
#include <string>

/* Simulation Vars */
const double STEP_TIME = 0.02;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);


void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }

}

int col_bullet_bubble (int s ,vector<Bullet> &bullets, vector<Bubble> &bubbles){

    //check for collision and destroy bubbles and bullets
    for (unsigned int i=0; i<bullets.size(); i++)
    {
        for (unsigned int j = 0 ; j<bubbles.size() ; j++)
        {
            double d;
            d = sqrt ( pow( bullets[i].get_center_x() - bubbles[j].get_center_x() , 2 )
                                                +
                       pow( bullets[i].get_center_y() - bubbles[j].get_center_y() , 2 ) ) ;

            if ( d <= 5 + bubbles[j].get_radius() )
            {
                // destroy bullet
                bullets.erase(bullets.begin()+i);

                // split bubble into two bubbles
                if ( bubbles[j].get_radius() > BUBBLE_DEFAULT_RADIUS )
                {
                    // creates two smaller bubbles moving opposite
                    bubbles.push_back(Bubble( bubbles[j].get_center_x() +  ( bubbles[j].get_radius() / 2 ) , bubbles[j].get_center_y() , bubbles[j].get_radius() / 2 , 2*bubbles[j].get_vx() , bubbles[j].get_vy() , COLOR(255,105,180)));
                    bubbles.push_back(Bubble( bubbles[j].get_center_x() -  ( bubbles[j].get_radius() / 2 ) , bubbles[j].get_center_y() , bubbles[j].get_radius() / 2 , -2*bubbles[j].get_vx() , bubbles[j].get_vy() , COLOR(255,105,180)));
                }

                // destroy original bubble
                bubbles.erase(bubbles.begin()+j);

                // increase score by 1
                s++ ;
            }
        }
    }
    return s ;
}

int col_bubble_shooter (int h , vector<Bubble> &bubbles , Shooter shooter )
{
    unsigned int i = 0;

    // reflect bubble
    while ( i < bubbles.size() && h > 0 )
        {
            // along x direction if collision with shooter body
            if
            (
                ( abs ( bubbles[i].get_center_x() - shooter.get_body_center_x() ) < bubbles[i].get_radius() + 8 )

                                                        &&

                ( bubbles[i].get_center_y() + bubbles[i].get_radius() > 410 - 4*sqrt(2) )
            )

                {
                    h-- ;
                    bubbles[i].reflect_bubble_rect() ;
                }

            // along both x and y direction if collision with shooter head
            else if
            (
                sqrt ( pow ( bubbles[i].get_center_x() - shooter.get_head_center_x() , 2 )
                                                      +
                       pow ( bubbles[i].get_center_y() - shooter.get_head_center_y() , 2 ) )

                                                        <

                                bubbles[i].get_radius() + 8
            )

                {
                        h--;
                        bubbles[i].reflect_bubble_head() ;
                }

            i++ ;
        }
    return h;
}

vector<Bubble> create_bubbles()
{
    // create initial bubbles in the game
    vector<Bubble> bubbles;
    bubbles.push_back(Bubble(WINDOW_X/2.0, BUBBLE_START_Y, 4 * BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
    bubbles.push_back(Bubble(WINDOW_X/4.0, BUBBLE_START_Y, 4 * BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
    return bubbles;
}


int main()
{
    int health = 3;
    double t = 0;
    int score = 0 ;

    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    /* Create dynamic time, health and score display */
    Text Time_display( 50 ,10, "Time:      / 60" ) ;
    Text Time_value (55,10, t ) ;
    Text Health_display ( 440 , 10 , "Health:    / 3" ) ;
    Text Health_value ( 455 , 10 , health ) ;
    Text Score_display ( 440 , 470 , " Score:   " ) ;
    Text Score_value ( 470 , 470 , score ) ;

    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));

    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);

    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    // Initialize the bubbles
    vector<Bubble> bubbles = create_bubbles();

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    XEvent event;

    // Main game loop
    while (true)
    {
        bool pendingEvent = checkEvent(event);
        if (pendingEvent)
        {
            // Get the key pressed
            char c = charFromEvent(event);
            msg_cmd[msg_cmd.length() - 1] = c;
            charPressed.setMessage(msg_cmd);

            // Update the shooter
            if(c == 'a')
                shooter.move(STEP_TIME, true);
            else if(c == 'd')
                shooter.move(STEP_TIME, false);
            else if(c == 'w')
                bullets.push_back(shooter.shoot());
            else if(c == 'q')
                return 0;
        }

        // Update the bubbles
        move_bubbles(bubbles);

        // Update the bullets
        move_bullets(bullets);

        // Update score if collision between bullet and bubble
        score = col_bullet_bubble( score , bullets , bubbles ) ;

        // Update health if collision between bubble and shooter
        health = col_bubble_shooter( health , bubbles , shooter ) ;

        /* Update the timer display */
        char time[3] ;
        int tim = t ;
        time[1] = '0' + ( tim % 10 ) ;
        tim = tim / 10 ;
        time[0] = '0' + ( tim % 10 ) ;
        Time_value.setMessage( time ) ;
        t = t + 0.02 ;

        /* Update the health display */
        ostringstream health_temp ;
        health_temp << health ;
        string Health_display_temp = health_temp.str() ;
        Health_value.setMessage ( Health_display_temp ) ;

        /* Upate the score display */
        ostringstream score_temp ;
        score_temp << score ;
        string Score_display_temp = score_temp.str() ;
        Score_value.setMessage ( Score_display_temp ) ;

        if ( floor(t-0.02) == 60 || health == 0 )
        {
            Text Lose ( 250 , 250 , "You Lose! Better Luck next time :)") ;
            wait(5);
            break ;
        }

        if ( score == 14 )
        {
            Text Win ( 250 , 250 , "You Win! (Probably good luck) " ) ;
            wait(5);
            break ;
        }

    }
}
