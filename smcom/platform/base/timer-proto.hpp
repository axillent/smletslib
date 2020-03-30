#ifndef SMCOM_TIMER_PROTO_HPP
#define SMCOM_TIMER_PROTO_HPP

class TimerProto {
public:
	static uint16_t Init(void);             // init returning resulting freq
	static void Start(void);                // start
	static void Stop(void);                 // stop
	static void ResetCounter();             // reset coubter
	static uint16_t GetCounter();           // getting counter
	static uint16_t GetCounterMax();        // get counter max
	static void SetCounterMax(uint16_t max) // set counter max
	static bool IsPendingUpdate();          // is pending update
	static uint16_t Scale();                    // shift scale
	static uint16_t ResetScale();               // reset scale
};

#endif

