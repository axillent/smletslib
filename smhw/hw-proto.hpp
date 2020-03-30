#ifndef SMHW_HW_PROTO_HPP
#define SMHW_HW_PROTO_HPP

class HWProto {
public:
  static void Init(smcom_funcp ptr);
  static void Start(void);
  static void Loop(void);
};

#endif
