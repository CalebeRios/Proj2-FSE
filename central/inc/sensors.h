#ifndef _SENSORS_H
#define _SENSORS_H

void config_mutex();
void update_all_outputs(Configuration *config);
void update_all_inputs(Configuration *config);
void update_temp_umi();
void update_people();

#endif
