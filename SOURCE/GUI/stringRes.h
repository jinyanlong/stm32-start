#ifndef STRING_H_0001
#define STRING_H_0001

#define S_NONE                                                       1
#define S_NORMAL                                                     2
#define S_EXCEPTION                                                  3
#define S_OKCANCEL                                                   4
#define S_YESNO                                                      5
#define S_PAGE_TIP                                                   6
#define S_COMPANY                                                    7
#define S_WEB                                                        8
#define S_PRODUCT                                                    9
#define S_INIT_PWD                                                   10
#define S_VALIDATE                                                   11
#define S_UNSUPPORT                                                  12
#define S_ON                                                         13
#define S_OFF                                                        14
#define S_FORMAT                                                     15
#define S_STARTING                                                   16
#define S_WAITING                                                    17
#define S_RUNNING                                                    18
#define S_CLOSED                                                     19
#define S_TAG_ONLINE                                                 20
#define S_TAG_OFFLINE                                                21
#define S_TAG_OFFLINING S_TAG_OFFLINE
#define S_NET_SENDING                                                22
#define S_NET_OK                                                     23
#define S_NET_FAILED                                                 24
////////////////////////MENU//////////////////////////////////////
#define S_MENU_UNSUPPORT                                             25
#define S_FORMTITLE_MAINMENU                                         26
#define S_MENU_MONITOR                                               27
#define S_MONITOR_TOTAL                                              28
#define S_MONITOR_UPLOAD_COUNT                                       29
#define S_MONITOR_TODAY_COUNT                                        30
#define S_MONITOR_TAG_COUNT                                          31
#define S_MONITOR_COIN_COUNT                                         32
#define S_MONITOR_GPS                                                33
#define S_MENU_RECORDLIST                                            34
#define S_RECORDLIST_TAGID                                           35
#define S_RECORDLIST_STATE                                           36
#define S_RECORDLIST_COIN                                            37
#define S_RECORDLIST_STEPER                                          38
#define S_RECORDLIST_STARTTIME                                       39
#define S_RECORDLIST_SAVETIME                                        40
#define S_MENU_TAGLIST                                               41
#define S_FORMTITLE_TAGLIST                                          42
#define S_MENU_COINLIST                                              43
#define S_FORMTITLE_COINLIST                                         44
#define S_MENU_DEVICE                                                45
#define S_DEVICE_SNR                                                 46
#define S_DEVICE_SYSID                                               47
#define S_WEB_IP                                                     48
////////////////´íÎó´úÂë/////////////////////
#define S_ERROR_PARAM_OVER                                           49
#define S_ERROE_TIMER                                                50
#define S_ERROE_TIMER_FORMAT                                         51
#define S_ERROE_FLASH                                                52
#define S_ERROE_FILE_FORMAT                                          53
#define S_ERROE_FILE_OVERSIZE                                        54
#define S_ERROE_SYSTEM                                               55
#define S_ERROE_OS                                                   56
#define S_ERROE_TAG                                                  57
#define S_ERROE_UNKNOWN                                              58
#define S_ERROE_TIP                                                  59
#define S_ERROE_TIMER_INVALIDATE                                     60
#define S_ERROE_TIMER_LOW_BATT                                       61
#define S_STRINGID_MAX                                               62

wstring LoadWString(int resId);
#endif
