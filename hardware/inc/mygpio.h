#ifndef MYGPIO_H
#define MYGPIO_H

#include "hc32_ll.h"

#define SYSRUN_G_PORT   (GPIO_PORT_C)
#define SYSRUN_G_PIN    (GPIO_PIN_13)

#define DeUpdate()         GPIO_SetPins(GPIO_PORT_C,GPIO_PIN_13)
#define Updating()         GPIO_ResetPins(GPIO_PORT_C,GPIO_PIN_13)


//输入信号
#define AIR_OK_PORT         (GPIO_PORT_B)
#define AIR_OK_PIN          (GPIO_PIN_15)
#define get_airok()         GPIO_ReadInputPins(AIR_OK_PORT,AIR_OK_PIN)      //气源OK信号

#define INSERT_PORT         (GPIO_PORT_D)
#define INSERT_PIN          (GPIO_PIN_12)
#define get_insert()        GPIO_ReadInputPins(INSERT_PORT,INSERT_PIN)      //插入信号

#define START_PORT          (GPIO_PORT_D)
#define START_PIN           (GPIO_PIN_13)
#define get_start()         GPIO_ReadInputPins(START_PORT,START_PIN)        //开始测试信号

#define IN4_PORT            (GPIO_PORT_A)
#define IN4_PIN             (GPIO_PIN_08)
#define get_in4()           GPIO_ReadInputPins(IN4_PORT,IN4_PIN)            //in4，保留

//输出信号

//空气选择阀，正压或者负压
#define SELECTAIR_PORT              (GPIO_PORT_F)
#define SELECTAIR_PIN               (GPIO_PIN_05)
#define get_selectair()             GPIO_ReadOutputPins(SELECTAIR_PORT,SELECTAIR_PIN)
#define set_selectair(n)            set_output(SELECTAIR_PORT,SELECTAIR_PIN,n)   

//正压保压阀
#define POSITIVEPREHOLD_PORT        (GPIO_PORT_F)
#define POSITIVEPREHOLD_PIN         (GPIO_PIN_04)
#define get_positiveprehold()       GPIO_ReadOutputPins(POSITIVEPREHOLD_PORT,POSITIVEPREHOLD_PIN)
#define set_positiveprehold(n)      set_output(POSITIVEPREHOLD_PORT,POSITIVEPREHOLD_PIN,n)

//负压保压阀
#define NEGATIVEPREHOLD_PORT        (GPIO_PORT_F)
#define NEGATIVEPREHOLD_PIN         (GPIO_PIN_03)
#define get_negativeprehold()       GPIO_ReadOutputPins(NEGATIVEPREHOLD_PORT,NEGATIVEPREHOLD_PIN)
#define set_negativeprehold(n)      set_output(NEGATIVEPREHOLD_PORT,NEGATIVEPREHOLD_PIN,n)

//测试结束输出信号
#define TESTEND_PORT                (GPIO_PORT_A)
#define TESTEND_PIN                 (GPIO_PIN_10)
#define get_testend()               GPIO_ReadOutputPins(TESTEND_PORT,TESTEND_PIN)
#define set_testend(n)              set_output(TESTEND_PORT,TESTEND_PIN,n)

//连通电磁阀
#define CONNECTAIR_PORT             (GPIO_PORT_F)
#define CONNECTAIR_PIN              (GPIO_PIN_09)
#define get_connectair()            GPIO_ReadOutputPins(CONNECTAIR_PORT,CONNECTAIR_PIN)
#define set_connectair(n)           set_output(CONNECTAIR_PORT,CONNECTAIR_PIN,n)

//测试OK输出信号
#define TESTOK_PORT                 (GPIO_PORT_F)
#define TESTOK_PIN                  (GPIO_PIN_08)
#define get_testok()                GPIO_ReadOutputPins(TESTOK_PORT,TESTOK_PIN)
#define set_testok(n)               set_output(TESTOK_PORT,TESTOK_PIN,n)

#define INDICATIONOK_PORT           (GPIO_PORT_F)
#define INDICATIONOK_PIN            (GPIO_PIN_07)
#define get_indicationok()          GPIO_ReadOutputPins(INDICATIONOK_PORT,INDICATIONOK_PIN)
#define set_indicationok(n)         set_output(INDICATIONOK_PORT,INDICATIONOK_PIN,n)

#define OT8_PORT                    (GPIO_PORT_F)
#define OT8_PIN                     (GPIO_PIN_06)
#define get_ot8()                   GPIO_ReadOutputPins(OT8_PORT,OT8_PIN)
#define set_ot8(n)                  set_output(OT8_PORT,OT8_PIN,n)

/**
 * @brief Initialize GPIO pins
 * @param None
 * @retval None
 */
void init_gpio(void);

void set_output(uint8_t u8Port, uint16_t u16Pin , uint8_t u8Value);

#endif