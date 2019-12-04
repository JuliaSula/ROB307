
double filterOutput;
double rotorOutput;
double  target_z;
double min_altitude;
double max_altitude;
bool custom_estimator_flies_the_drone;
bool motors_armed;
double yRef[6];

void runCustomEstimator(int commands[4], int proximity, int rfinders[6], int hgtMea, double target_climb_rate , double delVelCorrected[3],double delAngCorrected[3]) 
{
	//clean the temporary variables before we start the accumulation
	double stateNew=[0,0,0,0,0,0,0,0,0,0];
    filterOutput=[0,0,0,0];
	rotorOutput=[0,0,0,0];
	float yMeasured[6];
	//update input with latest downsampled data
	for (int i = 0; i < 3; i++) {
		yMeasured[i] = 100 * delVelCorrected[i]; //accel data *100 because delVelCorrected is multiplied by dt = 0.01s at 100Hz sampling rate
	}
	for (int i = 0; i < 3; i++) {
		yMeasured[i + 3] = 100 * delAngCorrected[i]; //gyro data *100 because delAngCorrected is multiplied by dt = 0.01s at 100Hz sampling rate
	}

	//DEBUG
	float m90 = -99, m45 = -99, m270 = -99, m315 =-99; //initial value different from actual multiflex measurements to tell if multiflex is working correctly

        //Ici appeler measures et references
	/*for (int i = 0; i < 3; i++) {
		yRef[i] =; //accel data
	}*/

	//defining references: we get RC controller inputs
	float command_roll, command_pitch, command_throttle, command_yaw ; //in centidegrees
	//get_pilot_desired_commands(command_roll, command_pitch, command_throttle, command_yaw, *_angle_max, *_angle_limit);
	double u_command = static_cast<double>(commands[0]); //conversion between angle and speed input. TODO: elaborate this function
	double v_command = static_cast<double>(commands[1]); //conversion between angle and speed input. TODO: elaborate this function
	double t_command = static_cast<double>(commands[2]); //conversion between angle and speed input. TODO: elaborate this function
	double y_command = static_cast<double>(commands[3]); //conversion between angle and speed input. TODO: elaborate this function
	t_command +=0; //to trick the compiler and avoid "unused variable" error
	y_command +=0; //to trick the compiler and avoid "unused variable" error
    yRef[0] = u_command; // velocity u(longitudinal) reference
    yRef[1] = v_command; //lateral shift reference

    //update z based on target_climb_rate from throttle
    //we update only if we are armed
    double gain_z_accumulation = 0.0001; //at 100Hz (EKF sampling) we increment 100 times per second
    if(motors_armed){
    	if(custom_estimator_flies_the_drone){
            target_z += target_climb_rate * gain_z_accumulation;
            //make sure we limit the altitude within min and max altitudes
            if(target_z>max_altitude){
            	target_z = max_altitude;
            }else if(target_z<min_altitude){
            	target_z = min_altitude;
            }else{
            	//pass, left target_z unchanged
            }
    	}
        //otherwise the target is the current measured height, this way if the custom estimator is suddendly put in control in mid-air the target_z will be equal to z_measured and there won't be any sudden altitude change
    	else{
        	target_z = static_cast<double>(hgtMea);
    	}
    }//else we are on the ground and the target_z should be 0
    else{
    	target_z = 0;
    }

	yRef[2] = target_z; //height reference
	yRef[3] = 0; //Deltapsi reference


	//Getting Values from RangeFinders
	//const uint8_t obj_count = _proximity->get_object_count();
	if(sum(rfinders)==0 ){
		//no proximity sensor connected, we skip the controller calculation
	}else{
		for(int i=0; i<6; i++){
			float angle, distance;
			if( proximity){
				switch(i){
					case 1:{
						rfinders[0] = static_cast<double>(distance);     //value got from the right45 range finder
						m45=static_cast<double>(distance); //for logging purposes to verify that the multiflex values are read correctly
						break;
					}
					case 2:{
						rfinders[1] = static_cast<double>(distance);     //value got from the right range finder
						m90=static_cast<double>(distance); //for logging purposes to verify that the multiflex values are read correctly
						break;
					}
					case 6:{
						rfinders[2] = static_cast<double>(distance);    //value got from the left range finder
						m270=static_cast<double>(distance); //for logging purposes to verify that the multiflex values are read correctly
						break;
					}
					case 7:{
						rfinders[3] = static_cast<double>(distance);     //value got from the left45 range finder
						m315=static_cast<double>(distance); //for logging purposes to verify that the multiflex values are read correctly
						break;
					}
					default:
						break;
				}
			}
		}
	}
}


int main( int argc, char** argv )
{
	
	yMeasured=[1,1,1,1,1,1];
	proximity=1;
	rangeFinders=[1,1,1,1];
	runCustomEstimator(float yMeasured, int proximity, int rangeFinders)
}
