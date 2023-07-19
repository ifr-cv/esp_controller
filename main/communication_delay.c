#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include "status.h"

#define CHECK_AMOUNT    65535//0-127检查的数量，也就是序号的变化范围
#define VALUE_ADD(A,B)  ((A+B)>=CHECK_AMOUNT ?   (A+B-CHECK_AMOUNT)  :    (A+B))
#define VALUE_SUB(A,B)  ( A<B ?   (A+CHECK_AMOUNT-B)  :    (A-B))
uint8_t seq_head,seq_tail,last_seq;
bool first_in =true ;

bool lost_flig[CHECK_AMOUNT]={0};
// TODO : 改善优化丢包率和延时计算算法 并反馈app端显示
uint8_t losstpock_ratio(uint8_t seqin)
{
    uint8_t lost_num;
    if(first_in)
    {
        first_in=false;
        last_seq=VALUE_SUB(seqin,1);
        seq_head=last_seq;
        seq_tail=seqin;
        lost_num=0;
    }
    seq_tail=seqin;

    if(seqin!=VALUE_ADD(last_seq,1))//丢包
    {
        lost_num=VALUE_SUB(VALUE_SUB(seqin,last_seq),1);//丢包个数
        for (uint8_t i = 0; i < lost_num; i++)//0到丢包个数-1
        {
            
            lost_flig[VALUE_SUB(VALUE_SUB(seqin,i),1)]=true;//记录所有丢包序号
        }
    }

    if(VALUE_SUB(seq_tail,seq_head)>=100)//只统计当前包前100包的范围
    {
        seq_head=VALUE_SUB(seq_tail,100);
    }
    uint8_t range=VALUE_SUB(seq_tail,seq_head);//
    lost_num=0;
    for (uint8_t i = 0; i < range; i++)//统计整个范围内丢了多少包
    {
        
        if(lost_flig[VALUE_SUB(VALUE_SUB(seqin,i),1)])
        lost_num++;
    }

    float lost_numf=lost_num,rangef=range;
    float lost_ratid=lost_numf/rangef;
    lost_ratid*=100;
    last_seq=seqin;

    return lost_ratid;
} 

int32_t missing = 0;

void communication_delay(void *arg)
{
    uint16_t last_report_seq_max = 256;
    uint16_t exp_since_last = 0;
    uint32_t rec_since_last = 0;
    uint32_t tmp_rec_count = 0;
	while (1) {
        // if(smsg.sequence < last_report_seq_max)
        // {
        //     exp_since_last = (65535 + smsg.sequence - last_report_seq_max);
        // }
        // else
        // {
            exp_since_last = (smsg.sequence - last_report_seq_max);        //两次发送间隔之间理论上应该收到的包数量=当前接收到的最大包序号-上个时刻最大有序包序号
        // }
        rec_since_last = rev_count - tmp_rec_count;     //两次发送间隔之间实际接收到有序包的数量=当前时刻收到的有序包的数量-上一个时刻收到的有序包的数量
        tmp_rec_count = rev_count;
        last_report_seq_max = smsg.sequence;
        missing = exp_since_last - rec_since_last;      //丢包数=理论上应收的包数-实际收到的包数
        vTaskDelay(2500 / portTICK_PERIOD_MS);

	}

}
