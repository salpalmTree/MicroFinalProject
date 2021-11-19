#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <driver/ledc.h>

#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"

const static char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
//const static char http_html_hdr1[] = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\n";
const static char http_index_hml[] = 
R"=====(<!DOCTYPE html><html> 
<head> 
<meta charset = UTF-8 name = "viewport" content = "width = device-width initial-scale = 1.0"> 
<title>Elevator System</title> 
</head>
<body> 
<div class="header"> 
<h1>Elevator System</h1> 
</div> 
<div class="thirdButtons">
<input class = "btn1" id = "btn0" type="button" value = "3rd Down" onclick = "sendRequest3down()">
<input class = "btn2" id = "btn1" type="button" value = "Third Floor" onclick = "sendRequestThird()">
</div>
<div class="secondButtons">
<input class = "btn3" id = "btn2" type="button" value = "2nd Up" onclick = "sendRequest2up()">
<input class = "btn4" id = "btn3" type="button" value = "2nd Down" onclick = "sendRequest2down()">
<input class = "btn5" id = "btn4" type="button" value = "Second Floor" onclick = "sendRequestSecond()">
</div>
<div class="firstButtons">
<input class = "btn6" id = "btn5" type="button" value = "1st Up" onclick = "sendRequest1up()">
<input class = "btn7" id = "btn6" type="button" value = "First Floor" onclick = "sendRequestFirst()">
</div>
<div class="ReqFloorButton">
<input class = "btn8" id = "btn7" type="button" value = "Current Floor" onclick = "sendRequestFloor()">
</div>
<label class = "switch">
<input id = "btn8" type="checkbox" onchange = "toggleCheckbox(this)"><span class = "slider"></span>
</label>
<style>
html {font-family: Arial; display: inline-block; text-align: center;} 
*{margin:0; padding:0;} body {background-color: #fbfbb6;} 
.header { width:100%; height:55px; color: white; background-color: #f44b37; padding: 0; text-align:center; } 
.header h1{ color:white; vertical-align:center; font-size:42px; } 
.thirdbuttons
{
    padding: 2px 100px; 
    overflow: hidden; 
}
.secondButtons
{
    padding: 2px 100px; 
    overflow: hidden; 
}
.firstButtons
{
    padding: 2px 100px; 
    overflow: hidden; 
}
.ReqFloorButton
{
    padding: 2px 100px; 
    overflow: hidden; 
}
.btn1
{
    margin: 5%; 
    margin-top: 2%;
    transition-duration: 0.4s;
    border-radius: 32px;
    border: 2px solid #000000;
    color: black; 
    padding: 10px 24px; 
    text-align: center; 
    text-decoration: none; 
    font-size: 24px; 
}
.btn1:hover
{background-color: #f44b37; color: white;}
.btn2
{
    margin: 5%; 
    margin-top: 2%;
    transition-duration: 0.4s;
    border-radius: 32px;
    border: 2px solid #000000;
    color: black; 
    padding: 10px 24px; 
    text-align: center; 
    text-decoration: none; 
    font-size: 24px;
}
.btn2:hover
{background-color: #6bfc03; color: white;}
.btn3
{
    margin: 5%;
    margin-top: 2%; 
    transition-duration: 0.4s;
    border-radius: 32px;
    border: 2px solid #000000;
    color: black; 
    padding: 10px 24px; 
    text-align: center; 
    text-decoration: none; 
    font-size: 24px;
}
.btn3:hover
{background-color: #f44b37; color: white;}
.btn4
{
    margin: 0%;
    margin-top: 2%; 
    transition-duration: 0.4s;
    border-radius: 32px;
    border: 2px solid #000000;
    color: black; 
    padding: 10px 24px; 
    text-align: center; 
    text-decoration: none; 
    font-size: 24px;
}
.btn4:hover
{background-color: #f44b37; color: white;}
.btn5
{
    margin: 5%;
    margin-top: 2%; 
    transition-duration: 0.4s;
    border-radius: 32px;
    border: 2px solid #000000;
    color: black; 
    padding: 10px 24px; 
    text-align: center; 
    text-decoration: none; 
    font-size: 24px;
}
.btn5:hover
{background-color: #6bfc03; color: white;}
.btn6
{
    margin: 5%;
    margin-top: 2%; 
    transition-duration: 0.4s;
    border-radius: 32px;
    border: 2px solid #000000;
    color: black; 
    padding: 10px 24px; 
    text-align: center; 
    text-decoration: none; 
    font-size: 24px;
}
.btn6:hover
{background-color: #f44b37; color: white;}
.btn7
{
    margin: 5%;
    margin-top: 2%; 
    transition-duration: 0.4s;
    border-radius: 32px;
    border: 2px solid #000000;
    color: black; 
    padding: 10px 24px; 
    text-align: center; 
    text-decoration: none; 
    font-size: 24px;    
}
.btn7:hover
{background-color: #6bfc03; color: white;}
.btn8
{
    margin: 5%;
    margin-top: 2%; 
    transition-duration: 0.4s;
    border-radius: 32px;
    border: 2px solid #000000;
    color: black; 
    padding: 10px 24px; 
    text-align: center; 
    text-decoration: none; 
    font-size: 24px;    
}
.btn8:hover
{background-color: #03a1fc; color: white;}
.switch {
    position: relative;
    display: inline-block;
    width: 60px;
    height: 34px;
    }
    .switch input {
        display: none;
    }
    .slider {
        position: absolute;
        cursor: pointer; 
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #ccc;
        border-radius: 8px;
    }
      .slider:before {
        position: absolute;
        content: "";
        height: 26px;
        width: 26px;
        left: 4px;
        bottom: 4px;
        background-color: #fff;
        -webkit-transition: 0.4s;
        transition: 0.4s;
        border-radius: 68px;
    }

    input:checked + .slider {
        background-color: #03a1fc;
    }
    input:checked + .slider:before {
        -webkit-transform: translateX(26px);
        -ms-transform: translateX(26px);
        transform: translateX(26px);
    }
</style> 
<script> 
function changeButton(value){ var btn = document.getElementById("btn7");
    if(value === "1")
    {
        btn.value = "First Floor";
    }
    else if(value === "2")
    {
        btn.value = "Second Floor";
    }
    else if(value === "3")
    {
        btn.value = "Third Floor"; 
    }
}
function sendRequestFloor(){var http = new XMLHttpRequest(); http.onreadystatechange = (() => { if(http.readyState === 4){ if(http.status === 200){ changeButton(http.responseText);}}}); 
http.open("GET", "f", true); http.send(); } 
function sendRequest3down(){ var http = new XMLHttpRequest(); http.onreadystatechange = (()=>{ if(http.readyState === 4){ if(http.status === 200){ var res = http.responseText; } } }); 
http.open("GET", "3d", true); http.send(); } 
function sendRequest2up(){ var http = new XMLHttpRequest(); http.onreadystatechange = (()=>{ if(http.readyState === 4){ if(http.status === 200){ var res = http.responseText; } } }); 
http.open("GET", "2up", true); http.send(); } 
function sendRequest2down(){ var http = new XMLHttpRequest(); http.onreadystatechange = (()=>{ if(http.readyState === 4){ if(http.status === 200){ var res = http.responseText; } } });
http.open("GET", "2d", true); http.send(); }
function sendRequest1up(){ var http = new XMLHttpRequest(); http.onreadystatechange = (()=>{ if(http.readyState === 4){ if(http.status === 200){ var res = http.responseText; } } });
http.open("GET", "1up", true); http.send(); }
function sendRequestThird(){ var http = new XMLHttpRequest(); http.onreadystatechange = (()=>{ if(http.readyState === 4){ if(http.status === 200){ var res = http.responseText; } } });
http.open("GET", "3", true); http.send(); }
function sendRequestSecond(){ var http = new XMLHttpRequest(); http.onreadystatechange = (()=>{ if(http.readyState === 4){ if(http.status === 200){ var res = http.responseText; } } });
http.open("GET", "2", true); http.send(); }
function sendRequestFirst(){ var http = new XMLHttpRequest(); http.onreadystatechange = (()=>{ if(http.readyState === 4){ if(http.status === 200){ var res = http.responseText; } } });
http.open("GET", "1", true); http.send(); }
function toggleCheckbox(element){var http = new XMLHttpRequest(); if(element.checked){http.open("GET", "B", true);} else{http.open("GET", "b", true);} http.send();}
</script> 
</body>
</html>)=====";
/*
    How Group Dispatcher Code works (gdCode): 

    represented as a binary number of 4 bits, 0b0000
    first and second bit represent floor number: 
        01 -> first floor
        10 -> second floor
        11 -> third floor
    last two bits represent direction: 
        11 -> up
        10 -> down
        00 -> no direction
    ex: 
        0b1001 -> direction is up from first floor




    dir u -> up
    dir d -> down
    dir s -> stay/tbd
*/
#define EXAMPLE_ESP_WIFI_SSID "ElESP"
#define EXAMPLE_ESP_WIFI_PASS "12345678"
#define EXAMPLE_MAX_STA_CONN 1
#define FIRSTFLOOR 23
#define SECONDFLOOR 4
#define THIRDFLOOR 21
#define BUZZ 18
int gdCode = 0b0000; 
bool Functional = true;

int curr = 1; 
int destination = 0; 
char dir = 'd'; 

typedef struct 
{
    int curFloor; 
    char direction; 
    int floorToGo; 
}Data_t;

uint32_t duty = 100; 
SemaphoreHandle_t FirstTask = NULL; //activate Task1
SemaphoreHandle_t readyToPack = NULL; //activate Task2
SemaphoreHandle_t readyToUnpack = NULL; //activate Task3
SemaphoreHandle_t status = NULL; //activate Task4
SemaphoreHandle_t statusTask1 = NULL; //status on Task1
SemaphoreHandle_t debugger = NULL; //Task 5
static QueueHandle_t packedData = NULL; 
static EventGroupHandle_t s_wifi_event_group; 
static const char *TAG = "wifi softAP";
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id)
    {
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG,"station:"MACSTR" join, AID=%d",
        MAC2STR(event->event_info.sta_connected.mac),
        event->event_info.sta_connected.aid); 
    break; 
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d",
        MAC2STR(event->event_info.sta_disconnected.mac),
        event->event_info.sta_disconnected.aid);
    break;
    default:
    break; 
    }
    return ESP_OK; 
}
void wifi_init_softap()
{
    s_wifi_event_group = xEventGroupCreate();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));  

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); 
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
            },
        };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}
static void http_server_netconn_serve(struct netconn *conn)
{
    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    err_t err;
    err = netconn_recv(conn, &inbuf);
    if (err == ERR_OK)
    {
        netbuf_data(inbuf, (void **)&buf, &buflen);
        if (buflen >= 5 && buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T' && buf[3] == ' ' && buf[4] == '/')
        {
            if(buf[5] == '3' && buf[6] == 'd')
            {
                gdCode = 0b1011;             //3 DOWN
                xSemaphoreGive(FirstTask); 
            }
            else if(buf[5] == '2' && buf[6] == 'u' && buf[7] == 'p')
            {
                gdCode = 0b1110;             //2 UP
                xSemaphoreGive(FirstTask); 
            }
            else if(buf[5] == '2' && buf[6] == 'd')
            {
                gdCode = 0b1010;           //2 DOWN
                xSemaphoreGive(FirstTask); 
            }
            else if(buf[5] == '1' && buf[6] == 'u' && buf[7] == 'p')
            {
                gdCode = 0b1101;            //1 UP
                xSemaphoreGive(FirstTask); 
            }
            else if(buf[5] == '3')
            {
                gdCode = 0b0011; 
                xSemaphoreGive(FirstTask); 
            }
            else if(buf[5] == '2')
            {
                gdCode = 0b0010; 
                xSemaphoreGive(FirstTask); 
            }  
            else if(buf[5] == '1')
            {
                gdCode = 0b0001; 
                xSemaphoreGive(FirstTask); 
            } 
            else if(buf[5] == 'f')
            {
                if(curr == 1)
                {
                    char toSend[] = "1"; 
                    netconn_write(conn, toSend, sizeof(toSend)-1, NETCONN_NOCOPY); 
                }
                else if(curr == 2)
                {
                    char toSend[] = "2"; 
                    netconn_write(conn, toSend, sizeof(toSend)-1, NETCONN_NOCOPY);    
                }
                else if(curr == 3)
                {
                    char toSend[] = "3"; 
                    netconn_write(conn, toSend, sizeof(toSend)-1, NETCONN_NOCOPY); 
                }
            }
            else if(buf[5] == 'B') 
            {
                gpio_set_level(BUZZ, 1); 
            }
            else if(buf[5] == 'b')
            {
                gpio_set_level(BUZZ, 0); 
            }
            else 
            {
                netconn_write(conn, http_html_hdr, sizeof(http_html_hdr)-1, NETCONN_NOCOPY);
                netconn_write(conn, http_index_hml, sizeof(http_index_hml)-1, NETCONN_NOCOPY);
            }
        }
    }
    netconn_close(conn);
    netbuf_delete(inbuf);
}

static void http_server(void *pvParameters)
{
    struct netconn *conn, *newconn;
    err_t err;
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, NULL, 80);
    netconn_listen(conn);
    do
    {
        err = netconn_accept(conn, &newconn);
        if (err == ERR_OK)
        {
            http_server_netconn_serve(newconn);
            netconn_delete(newconn);
        }
    } while (err == ERR_OK);
    netconn_close(conn);
    netconn_delete(conn);
}
void Task1(void *pvParameter)
{
    while(1)
    {
        if(xSemaphoreTake(FirstTask, portMAX_DELAY))
        {
            if(gdCode == 0b1011) //down from 3
            {
                destination = 3; 
                if(destination > curr)
                {
                    dir = 'u'; 
                    xSemaphoreGive(readyToPack); 
                }
            }
            else if(gdCode == 0b1110) //up from 2
            {
                destination = 2; 
                if(destination > curr)
                {
                    dir = 'u'; 
                    xSemaphoreGive(readyToPack); 
                }
                else 
                {
                    dir = 'd'; 
                    xSemaphoreGive(readyToPack); 
                }
            }
            else if(gdCode == 0b1010) //down form 2
            {
                destination = 2; 
                if(destination > curr)
                {
                    dir = 'u';
                    xSemaphoreGive(readyToPack); 
                }
                else
                {
                    dir = 'd'; 
                    xSemaphoreGive(readyToPack); 
                }
            }
            else if(gdCode == 0b1101) //up from 1
            {
                destination = 1; 
                if(destination < curr)
                {
                    dir = 'd'; 
                    xSemaphoreGive(readyToPack); 
                }
            }
            else if(gdCode == 0b0011) // go to 3
            {
                destination = 3; 
                dir = 's'; 
                xSemaphoreGive(readyToPack); 
            }
            else if(gdCode == 0b0010) // go to 2
            {
                destination = 2; 
                dir = 's'; 
                xSemaphoreGive(readyToPack); 
            }
            else if(gdCode == 0b0001) // go to 1
            {
                destination = 1; 
                dir = 's'; 
                xSemaphoreGive(readyToPack); 
            }
            duty = 100; 
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty); 
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0); 

        }
        if(xSemaphoreTake(statusTask1, portMAX_DELAY))
        {
            if(Functional == true)
            {
                printf("Good Operation\n");
            }
            else
            {
                printf("ERR"); 
            }
        }
    }
}
void Task2(void *pvParameter)
{
    BaseType_t xStatus; 
    while(1)
    {
        if(xSemaphoreTake(readyToPack, portMAX_DELAY))
        {
            Data_t StrucToSend; 
            StrucToSend.curFloor = curr; 
            StrucToSend.direction = dir; 
            StrucToSend.floorToGo = destination; 
            xStatus = xQueueSendToBack(packedData, &StrucToSend, 1000); 
            if(xStatus != pdPASS)
            {
                Functional = false; 
                xSemaphoreGive(status); 
            }
            else
            { 
                Functional = true; 
                xSemaphoreGive(readyToUnpack); 
                xSemaphoreGive(status); 

            }
        }
    }
}
void Task3(void *pvParameter)
{
    Data_t RecStruc; 
    BaseType_t xStatus; 
    while(1)
    {
        if(xSemaphoreTake(readyToUnpack, portMAX_DELAY))
        {
            xStatus = xQueueReceive(packedData, &RecStruc, portMAX_DELAY); 
            if(xStatus == pdPASS)
            {
                if(RecStruc.direction == 's')
                {
                    if(RecStruc.floorToGo > RecStruc.curFloor)
                    {
                        RecStruc.direction = 'u';
                    }
                    else if(RecStruc.floorToGo < RecStruc.curFloor)
                    {
                        RecStruc.direction = 'd'; 
                    }
                    else
                    {
                        RecStruc.direction = 's'; 
                    }
                }
                if(RecStruc.floorToGo == 3)
                {
                    if(RecStruc.curFloor == 1)
                    {
                        printf("going %c\n", RecStruc.direction); 
                        gpio_set_level(FIRSTFLOOR, 1); 
                        gpio_set_level(SECONDFLOOR, 0); 
                        gpio_set_level(THIRDFLOOR, 0);  
                        vTaskDelay(500/portTICK_PERIOD_MS); 

                        gpio_set_level(FIRSTFLOOR, 0); 
                        gpio_set_level(SECONDFLOOR, 1); 
                        vTaskDelay(500/portTICK_PERIOD_MS); 

                        gpio_set_level(SECONDFLOOR, 0); 
                        gpio_set_level(THIRDFLOOR, 1); 

                    }
                    else if(RecStruc.curFloor == 2)
                    {
                        printf("going %c\n", RecStruc.direction);
                        gpio_set_level(SECONDFLOOR, 1); 
                        gpio_set_level(FIRSTFLOOR, 0); 
                        gpio_set_level(THIRDFLOOR, 0); 
                        vTaskDelay(500/portTICK_PERIOD_MS); 

                        gpio_set_level(SECONDFLOOR, 0); 
                        gpio_set_level(THIRDFLOOR, 1); 
                    }
                    else 
                    {
                        printf("going %c\n", RecStruc.direction);
                        gpio_set_level(THIRDFLOOR, 1); 
                    }
                }
                else if(RecStruc.floorToGo == 2)
                {
                    if(RecStruc.curFloor == 3)
                    {
                        printf("going %c\n", RecStruc.direction);
                        gpio_set_level(THIRDFLOOR, 1); 
                        gpio_set_level(SECONDFLOOR, 0); 
                        gpio_set_level(FIRSTFLOOR, 0); 
                        vTaskDelay(500/portTICK_PERIOD_MS); 

                        gpio_set_level(THIRDFLOOR, 0); 
                        gpio_set_level(SECONDFLOOR, 1); 
                    }
                    else if(RecStruc.curFloor == 1)
                    {
                        printf("going %c\n", RecStruc.direction);
                        gpio_set_level(FIRSTFLOOR, 1); 
                        gpio_set_level(SECONDFLOOR, 0); 
                        gpio_set_level(THIRDFLOOR, 0); 
                        vTaskDelay(500/portTICK_PERIOD_MS); 

                        gpio_set_level(FIRSTFLOOR, 0); 
                        gpio_set_level(SECONDFLOOR, 1); 
 
                    }
                    else
                    {
                        printf("going %c\n", RecStruc.direction);
                        gpio_set_level(SECONDFLOOR, 1); 
                    }
                }
                else
                {
                    if(RecStruc.curFloor == 3)
                    {
                        printf("going %c\n", RecStruc.direction);
                        gpio_set_level(THIRDFLOOR, 1); 
                        gpio_set_level(SECONDFLOOR, 0); 
                        gpio_set_level(FIRSTFLOOR, 0); 
                        vTaskDelay(500/portTICK_PERIOD_MS);  

                        gpio_set_level(THIRDFLOOR, 0); 
                        gpio_set_level(SECONDFLOOR, 1); 
                        vTaskDelay(500/portTICK_PERIOD_MS); 

                        gpio_set_level(SECONDFLOOR, 0); 
                        gpio_set_level(FIRSTFLOOR, 1);                     
                    }
                    else if(RecStruc.curFloor == 2)
                    {
                        printf("going %c\n", RecStruc.direction);
                        gpio_set_level(SECONDFLOOR, 1); 
                        gpio_set_level(THIRDFLOOR, 0); 
                        gpio_set_level(FIRSTFLOOR, 0); 
                        vTaskDelay(500/portTICK_PERIOD_MS); 

                        gpio_set_level(SECONDFLOOR, 0); 
                        gpio_set_level(FIRSTFLOOR, 1); 

                    }
                    else
                    {
                        printf("going %c\n", RecStruc.direction);
                        gpio_set_level(FIRSTFLOOR, 1);  
                    }
                }
                vTaskDelay(500/portTICK_PERIOD_MS); 
                curr = RecStruc.floorToGo; 
                duty = 500; 
                printf("DOORS ARE OPEN\n");
                ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty); 
                ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0); 
            }
        }
    }
}
void Task4(void *pvParameter)
{
    while(1)
    {
        if(xSemaphoreTake(status, portMAX_DELAY))
        {
            if(Functional == true)
            {
                xSemaphoreGive(statusTask1); 
            }
            else if(Functional == false)
            {
                xSemaphoreGive(statusTask1); 
                xSemaphoreGive(debugger); 
                printf("Please contact elevator support, its busted\n"); 
            }
        }
    }
}
void Task5(void *pvParameter)
{
    while(1)
    {
        printf("Nothing to debug yet\n");
        if(xSemaphoreTake(debugger, 1000/portTICK_PERIOD_MS))
        {
            printf("I am debugging\n"); 
        }
        vTaskDelay(750/portTICK_PERIOD_MS); 
    }
}
void gpioSetUp()
{
    gpio_pad_select_gpio(FIRSTFLOOR);
    gpio_pad_select_gpio(SECONDFLOOR); 
    gpio_pad_select_gpio(THIRDFLOOR); 
    gpio_pad_select_gpio(BUZZ); 
    gpio_set_direction(FIRSTFLOOR, GPIO_MODE_OUTPUT);
    gpio_set_level(FIRSTFLOOR, 1); 
    gpio_set_direction(SECONDFLOOR, GPIO_MODE_OUTPUT); 
    gpio_set_level(SECONDFLOOR, 0); 
    gpio_set_direction(THIRDFLOOR, GPIO_MODE_OUTPUT); 
    gpio_set_level(THIRDFLOOR, 0);  
    gpio_set_direction(BUZZ, GPIO_MODE_OUTPUT); 
    gpio_set_level(BUZZ, 0); 
}
void pwmSetUp()
{
    ledc_channel_config_t tChaConfig; 
    tChaConfig.gpio_num = GPIO_NUM_5;
    tChaConfig.speed_mode = LEDC_HIGH_SPEED_MODE; 
    tChaConfig.channel = LEDC_CHANNEL_0; 
    tChaConfig.intr_type = LEDC_INTR_DISABLE; 
    tChaConfig.timer_sel = LEDC_TIMER_0; 
    tChaConfig.duty = 100;
    ledc_channel_config(&tChaConfig); 

    ledc_timer_config_t timerConfig; 
    timerConfig.duty_resolution = LEDC_TIMER_12_BIT;  
    timerConfig.timer_num = LEDC_TIMER_0;
    timerConfig.freq_hz = 50; 
    timerConfig.speed_mode = LEDC_HIGH_SPEED_MODE; 
    ledc_timer_config(&timerConfig); 
}
void app_main(void)
{
    packedData = xQueueCreate(5, sizeof(Data_t)); 
    FirstTask = xSemaphoreCreateBinary();  
    readyToPack = xSemaphoreCreateBinary();
    readyToUnpack = xSemaphoreCreateBinary(); 
    status = xSemaphoreCreateBinary(); 
    statusTask1 = xSemaphoreCreateBinary(); 
    debugger = xSemaphoreCreateBinary(); 
    nvs_flash_init();
    wifi_init_softap();
    gpioSetUp(); 
    pwmSetUp(); 
    xTaskCreate(&http_server, "http_server", 4096, NULL, 5, NULL); 
    xTaskCreate(&Task1, "Task1", 1024, NULL, 5, NULL); 
    xTaskCreate(&Task2, "Task2", 2084, NULL, 4, NULL); 
    xTaskCreate(&Task3, "Task3", 2048, NULL, 3, NULL); 
    xTaskCreate(&Task4, "Task4", 2048, NULL, 2, NULL); 
    xTaskCreate(&Task5, "Task5", 2048, NULL, 1, NULL); 
}
