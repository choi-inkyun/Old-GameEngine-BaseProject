
#ifndef __DEFINE_H__
#define __DEFINE_H__

#define MAX_USERID				(12+1)		/* 사용자 ID 최대 길이		*/
//#define ANIMATION_MOVING_FRAMES	( IS_FAST_MOVING ?   20 :  40 )		// 애니메이션 프레임 수
//#define ANIMATION_MOVING_TIME	( IS_FAST_MOVING ?  100 : 200 )		// 100 = 0.1 초
//#define ANIMATION_MARGIN_FRAMES	( IS_FAST_MOVING ?    2 :   2 )		// 애니메이션 전후 프레임 수

#define IS_FAST_MOVING			m_bOptions[4]

#define MAX_CHANNELS			12			// 사운드 채널 갯수
#define MAX_OPTIONS				5			// 옵션 갯수

#define MAX_CHAT_MESSAGE		64

#endif