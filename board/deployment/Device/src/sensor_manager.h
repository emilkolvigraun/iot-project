// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif
    typedef void (*button_cb)(void *);
 
    void initSensors();
    float get_temperature();
    float get_humidity();
    float get_ambientLight();

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_MANAGER_H */