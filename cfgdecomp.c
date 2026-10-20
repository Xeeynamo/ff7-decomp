void func_801D080C(s32 arg0) {
    RECT sp18;
    RECT sp20;
    RECT padding[2];
    s16 temp_s0;
    s32 temp_s0_9;
    s32 temp_s3;
    s32 x;
    s32 y;
    s32 temp_v1;
    s32 var_a1_3;
    s32 i;
    u16 var_s2_3;
    s32* var_a3;

    D_801D24B8.press = func_8001C808();
    D_801D24BC = D_801D24B8.press & (D_801D24B8.prev ^ D_801D24B8.press);
    func_800230C4(D_80062F58);
    switch (D_801D1AA8) {
    case 0: // config menu
        func_80026F44(16, 11, func_80015248(8, D_801D24CC[0].unkB + 0x37, 8), 7);
        if (D_801D24CC[0].unkB == 0) {
            func_8001EB2C(12, D_801D24A0[1].y + 0x10);
        } else {
            func_8001EB2C(12, D_801D24A0[1].y + 0x23 + (D_801D24CC[0].unkB - 1) * 18);
        }
        break;
    case 1: // window color sub-menu
        if (arg0 & 2) {
            if (D_801D24CC[0].unkB == 0) {
                func_8001EB2C(12, D_801D24A0[1].y + 0x10);
            } else {
                func_8001EB2C(12, D_801D24A0[1].y + 0x25 + (D_801D24CC[0].unkB - 1) * 18);
            }
        }
        func_8001EB2C(D_801D24CC[1].unkA * 0x3D + 0x8D, D_801D24A0[1].y + 4 + D_801D24CC[1].unkB * 18);
        break;
    case 2: // RGB sub-menu
        if (arg0 & 2) {
            func_8001EB2C(12, D_801D24A0[1].y + 0x10 + D_801D24CC[0].unkB * 18);
            func_8001EB2C(D_801D24CC[1].unkA * 0x3D + 0x8D, D_801D24A0[1].y + 4 + D_801D24CC[1].unkB * 19);
        }
        func_8001EB2C(0x93, D_801D24A0[1].y + 0x24 + D_801D24CC[2].unkB * 12);
        temp_s0 = D_801D24A0[1].y;
        temp_s3 = D_801D24A0[1].x + 0xAD;
        func_8002708C(temp_s3, temp_s0 + 0x23, D_801D2478[0][0], 2);
        func_8002708C(temp_s3, temp_s0 + 0x2F, D_801D2478[1][0], 4);
        func_8002708C(temp_s3, temp_s0 + 0x3B, D_801D2478[2][0], 1);
        
        for (i = 0; i < 3; i++) {
            func_80029114(
                temp_s3 + 0x10,
                temp_s0 + 0x23 + i * 12,
                D_801D252C[D_801D24CC[1].unkA * 3 + D_801D24CC[1].unkB * 6 + i],
                3, 7);
            sp20.x = D_801D252C[D_801D24CC[1].unkA * 3 + D_801D24CC[1].unkB * 6 + i] / 2 + 0xD5;
            sp20.y = D_801D24A0[1].y + 0x21 + i * 12;
            sp20.w = 8;
            sp20.h = 0xB;
            func_80028030(&sp20);
            sp20.x = 0xD5;
            sp20.y = D_801D24A0[1].y + 0x21 + i * 12;
            sp20.w = 0x88;
            sp20.h = 0xB;
            func_80027B84(&sp20);
        }

        sp20.x = 0;
        sp20.y = 0;
        sp20.w = 0x100;
        sp20.h = 0x100;
        func_80026A34(0, 1, 0x1F, &sp20);
        func_8001DE0C(&sp18, D_801D24A0[1].x + 0xA5, D_801D24A0[1].y + 0x1D, 0xC0, 0x2B);
        func_8001E040(&sp18);
        func_8001DE0C(&sp18, D_801D24A0[1].x + 0xF8, D_801D24A0[1].y + 7, 0x1A, 18);
        temp_v1 = D_801D24CC[1].unkB * 6 + D_801D24CC[1].unkA * 3;
        func_8001DF24(&sp18, D_801D252C[temp_v1], D_801D252C[temp_v1+1], D_801D252C[temp_v1+2]);
        func_8001DE0C(&sp18, D_801D24A0[1].x + 0xF5, D_801D24A0[1].y + 4, 0x20, 0x18);
        func_8001E040(&sp18);
        break;
    case 3: // controller customization sub-menu
        if (arg0 & 2) {
            func_8001EB2C(12, D_801D24A0[1].y + 10 + D_801D24CC[0].unkB * 0x15);
        }
        if (D_801D2528 == 0) {
            func_80026F44(0x10, 0xB, D_801D23E8, 7);
        } else {
            func_80026F44(0x10, 0xB, D_801D2418, 7);
        }
        x = D_801D24A0[1].x + 0x8C;
        y = D_801D24A0[1].y + 3;
        if (D_801D2528 != 0) {
            func_8001EB2C(D_801D24A0[1].x + 0x78, y + 10 + (D_801D2528 - 1) * 18);
        }
        for (i = 0; i < 10; i++) {
            func_8002708C(x + 0x22, y + 10 + i * 18, 0xDA, 5);
        }
        func_8001DE0C(&sp18, x, y, 0x78, 0xBB);
        y += 6;
        x += 10;
        for (i = 0; i < 10; i++) {
            func_801D014C(x, y + i * 18, D_801D1ADC[i]);
            func_801D014C(x + 0x2C, y + i * 18, _work.button_config[D_801D1ADC[i]]);
            sp20.x = 0;
            sp20.y = 0;
            sp20.w = 0x100;
            sp20.h = 0x100;
            func_80026A34(0, 1, 0x1F, &sp20);
            func_80026F44(x + 0x3E, y + 4 + i * 18 - 2, D_801D2238[_work.button_config[D_801D1ADC[i]]], 6);
        }
        func_8001E040(&sp18);
        break;
    }
    func_801D01C8();
    sp20.x = 0;
    sp20.y = 0;
    sp20.w = 0xFF;
    sp20.h = 0xFF;
    func_80026A34(0, 1, 0x1F, &sp20);
    func_8001DE70();
    func_8001DEF0(D_801D252C);
    func_8001DE0C(&sp18, D_801D24A0[1].x + 0xA5, D_801D24A0[1].y + 4, 0x40, 0x18);
    func_8001E040(&sp18);
    func_8001DEB0();
    for (i = 0; i < 2; i++) {
        func_8001E040(&D_801D24A0[i]);
    }
    if (func_80023050() == 0) {
        func_800264A8(&D_801D24CC[D_801D1AA8]);
        switch (D_801D1AA8) {
        case 4:
            break;
        case 0:
            if (D_80062D7E & 0x40) {
                func_801D0040(4);
                func_8002305C(5, 0);
                func_8002120C(0);
            } else {
                switch (D_801D24CC[0].unkB) {
                case 0: // window color
                    if (D_80062D7C & 0x20) {
                        D_801D1AA8 = 1;
                        func_801D0040(1);
                    }
                    break;
                case 1: // sound
                    var_s2_3 =_work.config & 3;
                    if (D_80062D7E & 0x2000) {
                        if (var_s2_3 != 1) {
                            func_801D0040(1);
                            var_s2_3++;
                            _work.config &= 0xFFFC;
                            _work.config |= var_s2_3 << 0;
                            func_801D0080(var_s2_3);
                        }
                    }
                    if (D_80062D7E & 0x8000) {
                        if (var_s2_3 != 0) {
                            func_801D0040(1);
                            var_s2_3--;
                            _work.config &= 0xFFFC;
                            _work.config |= var_s2_3 << 0;
                            func_801D0080(var_s2_3);
                        }
                    }
                    break;
                case 2: // controller
                    var_s2_3 = (_work.config >> 2) & 3;
                    if (D_80062D7C & 0x20) {
                        if (var_s2_3 == 1) {
                            func_801D0040(1);
                            D_801D1AA8 = 3;
                            D_801D2528 = 0;
                        }
                    } else {
                        if (D_80062D7E & 0x2000) {
                            if (var_s2_3 != 1) {
                                func_801D0040(1);
                                var_s2_3++;
                                _work.config &= 0xFFF3;
                                _work.config |= var_s2_3 << 2;
                            }
                        }
                        if (D_80062D7E & 0x8000) {
                            if (var_s2_3 != 0) {
                                func_801D0040(1);
                                var_s2_3--;
                                _work.config &= 0xFFF3;
                                _work.config |= var_s2_3 << 2;
                            }
                        }
                    }
                    break;
                case 3: // cursor
                    var_s2_3 = (_work.config >> 4) & 3;
                    if (D_80062D7E & 0x2000) {
                        if (var_s2_3 != 1) {
                            func_801D0040(1);
                            var_s2_3++;
                            _work.config &= 0xFFCF;
                            _work.config |= var_s2_3 << 4;
                        }
                    }
                    if (D_80062D7E & 0x8000) {
                        if (var_s2_3 != 0) {
                            func_801D0040(1);
                            var_s2_3--;
                            _work.config &= 0xFFCF;
                            _work.config |= var_s2_3 << 4;
                        }
                    }
                    break;
                case 4: // ATB
                    var_s2_3 = (_work.config >> 6) & 3;
                    if (D_80062D7E & 0x2000) {
                        if (var_s2_3 != 2) {
                            var_s2_3++;
                            func_801D0040(1);
                            _work.config &= 0xFF3F;
                            _work.config |= var_s2_3 << 6;
                        }
                    }
                    if (D_80062D7E & 0x8000) {
                        if (var_s2_3 != 0) {
                            var_s2_3--;
                            func_801D0040(1);
                            _work.config &= 0xFF3F;
                            _work.config |= var_s2_3 << 6;
                        }
                    }
                    break;
                case 5: // battle speed
                    if (D_80062D78 & 0x2000) {
                        if (_work.battle_speed != 0xFF) {
                            if (arg0 & 4) {
                                func_801D0040(1);
                            }
                            _work.battle_speed += 1;
                        }
                    }
                    if (D_80062D78 & 0x8000) {
                        if (_work.battle_speed != 0) {
                            if (arg0 & 4) {
                                func_801D0040(1);
                            }
                            _work.battle_speed -= 1;
                        }
                    }
                    break;
                case 6: // battle message
                    if (D_80062D78 & 0x2000) {
                        if (_work.battle_msg_speed != 0xFF) {
                            if (arg0 & 4) {
                                func_801D0040(1);
                            }
                            _work.battle_msg_speed += 1;
                        }
                    }
                    if (D_80062D78 & 0x8000) {
                        if (_work.battle_msg_speed != 0) {
                            if (arg0 & 4) {
                                func_801D0040(1);
                            }
                            _work.battle_msg_speed -= 1;
                        }
                    }
                    break;
                case 7: // field message
                    if (D_80062D78 & 0x2000) {
                        if (_work.field_msg_speed != 0xFF) {
                            if (arg0 & 4) {
                                func_801D0040(1);
                            }
                            _work.field_msg_speed += 1;
                        }
                    }
                    if (D_80062D78 & 0x8000) {
                        if (_work.field_msg_speed != 0) {
                            if (arg0 & 4) {
                                func_801D0040(1);
                            }
                            _work.field_msg_speed -= 1;
                        }
                    }
                    break;
                case 8: // camera angle
                    var_s2_3 = (_work.config >> 8) & 3;
                    if (D_80062D7E & 0x2000) {
                        if (var_s2_3 != 1) {
                            func_801D0040(1);
                            var_s2_3++;
                            _work.config &= ~0x300;
                            _work.config |= var_s2_3 << 8;
                        }
                    }
                    if (D_80062D7E & 0x8000) {
                        if (var_s2_3 != 0) {
                            func_801D0040(1);
                            var_s2_3--;
                            _work.config &= ~0x300;
                            _work.config |= var_s2_3 << 8;
                        }
                    }
                    break;
                case 9: // magic order
                    var_s2_3 = (_work.config >> 10) & 7;
                    if (D_80062D7E & 0x2000) {
                        if (var_s2_3 != 5) {
                            func_801D0040(1);
                            var_s2_3++;
                            _work.config &= ~0x1C00;
                            _work.config |= var_s2_3 << 10;
                        }
                    }
                    if (D_80062D7E & 0x8000) {
                        if (var_s2_3 != 0) {
                            func_801D0040(1);
                            var_s2_3--;
                            _work.config &= ~0x1C00;
                            _work.config |= var_s2_3 << 10;
                        }
                    }
                    break;
                }
            }
            break;
        case 1:
            if (D_80062D7C & 0x40) {
                for (i = 0; i < 12;i++) {
                    D_80049208[i] = D_801D252C[i];
                }
                func_801D0040(4);
                D_801D1AA8 = 0;
            } else if (D_80062D7C & 0x20) {
                D_801D1AA8 = 2;
                func_801D0040(1);
            }
            break;
        case 2:
            temp_s0_9 = D_801D24CC[1].unkA * 3 + D_801D24CC[1].unkB * 6 + D_801D24CC[2].unkB;
            if (D_80062D7C & 0x40) {
                func_801D0040(4);
                D_801D1AA8 = 1;
            } else if (D_80062D78 & 0x2000) {
                if (D_801D252C[temp_s0_9] != 0xFF) {
                    if (arg0 & 4) {
                        func_801D0040(1);
                    }
                    D_801D252C[temp_s0_9]++;
                }
            } else if (D_80062D78 & 0x8000) {
                if (D_801D252C[temp_s0_9] != 0) {
                    if (arg0 & 4) {
                        func_801D0040(1);
                    }
                    D_801D252C[temp_s0_9]--;
                }
            }
            break;
        case 3:
            if (D_801D24BC & 0x40) {
                if (D_801D2528 == 0) {
                    func_801D0040(4);
                    D_801D1AA8 = 0;
                    break;
                }
            } else if (D_801D2528 == 0) {
                if (D_801D24BC & 0x800) {
                    D_801D2528 = 1;
                }
                break;
            }
            if (D_801D24BC != 0) {
                if ((u8)func_801D0118(D_801D24BC) != 0xFF) {
                    _work.button_config[D_801D1ADC[D_801D2528-1]] = func_801D0118(D_801D24BC);
                } else if ((D_80062D7E & 0x1000) && D_801D2528 != 1) {
                    func_801D0040(1);
                    D_801D2528--;
                } else if ((D_80062D7E & 0x4000) && D_801D2528 != 10) {
                    func_801D0040(1);
                    D_801D2528++;
                } else {
                    if (D_80062D7E & 0xA000) {
                        var_a1_3 = 0;
                        var_s2_3 = 0;
                        var_a3 = D_801D24C0;
                        for (; var_s2_3 < 3; var_s2_3++) {
                            for (i = 0; i < 16; i++) {
                                if (var_a3[var_s2_3] == _work.button_config[i]) {
                                    var_a1_3 |= 1 << var_s2_3;
                                }
                            }
                        }
                        if (var_a1_3 == 7) {
                            func_801D0040(1);
                            D_801D2528 = 0;
                            D_801D1AA8 = 0;
                        } else {
                            func_801D0040(3);
                        }
                    }
                }
            }
            break;
        }
    }
    D_801D24B8.prev = D_801D24B8.press;
}
