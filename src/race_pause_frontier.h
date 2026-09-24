#ifndef RRJ_RACE_PAUSE_FRONTIER_H
#define RRJ_RACE_PAUSE_FRONTIER_H

#include "race_pause.h"
#include "race_leaf.h"

uint32_t sub_800CB4F8(RRJMemory *m, int32_t delta, RRJRaceLeafCall call);
uint32_t sub_800C8B24(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_800C8CD4(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_8004D184(RRJMemory *m, int32_t x, int32_t y);
uint32_t sub_8001E084(RRJMemory *m);
uint32_t sub_8001C304(RRJMemory *m, int32_t x, int32_t y, int32_t width,
                      int32_t height, uint32_t ordering_slot);
uint32_t sub_8005FA68(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t next_index, uint32_t packet_index);
uint32_t sub_800C5618(RRJMemory *m, uint32_t player_index);
uint32_t sub_8005FAC4(RRJMemory *m, uint32_t packet_base, uint32_t state,
                      uint32_t player_index);
uint32_t sub_80095410(RRJMemory *m, uint32_t actor);
uint32_t sub_8008B84C(RRJMemory *m, uint32_t actor);
uint32_t sub_8005F9B0(RRJMemory *m, uint32_t packets, int32_t delta,
                      int32_t first, int32_t last);
uint32_t sub_800C5558(RRJMemory *m, uint32_t player_index);
uint32_t sub_8005F834(RRJMemory *m, uint32_t packets, uint32_t state,
                      uint32_t reverse);
uint32_t sub_80063408(RRJMemory *m, uint32_t actor, uint32_t packets,
                      uint32_t animation, uint32_t player_index,
                      uint32_t hide_second);
uint32_t sub_800C5168(RRJMemory *m, uint32_t vertices, uint32_t color,
                      uint32_t mode, uint32_t player_index);
uint32_t sub_8001FE80(RRJMemory *m, int32_t angle, uint32_t sine_output,
                      uint32_t cosine_output);
uint32_t sub_8002DEC8(RRJMemory *m, uint32_t vector, int32_t scale);
uint32_t sub_8002DE40(RRJMemory *m, int32_t angle, uint32_t vector);
uint32_t sub_800C5380(RRJMemory *m, uint32_t position, int32_t angle,
                      uint32_t player_index, uint32_t marker_type);
uint32_t sub_80013E64(RRJMemory *m, uint32_t state);
uint32_t sub_80013B90(RRJMemory *m, int32_t value, uint32_t packets,
                      uint32_t table);
uint32_t sub_80013AF8(RRJMemory *m, uint32_t packet, uint32_t template_data);
uint32_t sub_800C569C(RRJMemory *m, uint32_t subject, uint32_t type_filter,
                      int32_t angle, uint32_t player_index);
uint32_t sub_800C52A8(RRJMemory *m, uint32_t actor, uint32_t player_index,
                      uint32_t inhibited);
uint32_t sub_800C52A0(RRJMemory *m, uint32_t actor, uint32_t player_index);
uint32_t sub_80061E50(RRJMemory *m, uint32_t state, uint32_t player_index,
                      uint32_t configuration);
uint32_t sub_8005FE58(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor);
uint32_t sub_80016528(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_8005FF84(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor, RRJRaceLeafCall call);
uint32_t sub_80060178(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor);
uint32_t sub_800603E4(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t player_index);
uint32_t sub_800606F8(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor, uint32_t flags,
                      uint32_t player_index, uint32_t blink_state,
                      uint32_t game_state);
uint32_t sub_800606F0(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor, uint32_t flags,
                      uint32_t player_index, uint32_t blink_state);
uint32_t sub_80060C18(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor, uint32_t player_index,
                      uint32_t configuration, uint32_t blink_state,
                      uint32_t display_flags);
uint32_t sub_80060C10(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor, uint32_t player_index,
                      uint32_t configuration, uint32_t blink_state);
uint32_t sub_80061494(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor, uint32_t player_index,
                      uint32_t configuration, uint32_t blink_state,
                      uint32_t display_flags);
uint32_t sub_8006148C(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor, uint32_t player_index,
                      uint32_t configuration, uint32_t blink_state);
uint32_t sub_80061F6C(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor, uint32_t player_index);
uint32_t sub_80062368(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor, uint32_t animation);
uint32_t sub_80062618(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t suppressed, uint32_t game_state);
uint32_t sub_80062610(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t unused, uint32_t suppressed);
uint32_t sub_80062C40(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t cache, uint32_t actor);
uint32_t sub_80062D9C(RRJMemory *m, uint32_t packets, uint32_t state,
                      uint32_t actor, uint32_t player_index);
uint32_t sub_8004CE44(RRJMemory *m, uint32_t packet, uint32_t dither,
                      uint32_t draw_to_display, uint32_t page,
                      uint32_t texture_window);
uint32_t sub_80062F34(RRJMemory *m, uint32_t packets, uint32_t actor,
                      uint32_t state, uint32_t player_index);
uint32_t sub_80063530(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t cache, uint32_t state, uint32_t actor,
                      uint32_t countdown, uint32_t suppressed);
uint32_t sub_800636F0(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor, uint32_t suppressed);
uint32_t sub_8005F030(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor, uint32_t player_index,
                      uint32_t suppressed);
uint32_t sub_8005FB4C(RRJMemory *m, uint32_t ordering_slot, uint32_t packets,
                      uint32_t state, uint32_t actor, uint32_t input_flags,
                      uint32_t animation, uint32_t player_index);
uint32_t sub_8003B9A8(RRJMemory *m, int32_t owner, uint32_t kind);
uint32_t sub_8003BC48(RRJMemory *m, int32_t owner, int32_t id,
                      uint32_t output, int32_t limit);
uint32_t sub_8003C590(RRJMemory *m, uint32_t actor);
uint32_t sub_8005E850(RRJMemory *m, uint32_t enabled, RRJRaceLeafCall call);
uint32_t sub_8005E848(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_80064B9C(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_800650D0(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_800654B4(RRJMemory *m, int32_t route, int32_t position);
uint32_t sub_800656A8(RRJMemory *m, int32_t point, int32_t first_start, int32_t second_start,
                      int32_t second_end);
uint32_t sub_800662BC(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_80065174(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_800662C4(RRJMemory *m, uint32_t manager, RRJRaceLeafCall call);
uint32_t sub_80020400(RRJMemory *m, uint32_t input, uint32_t output);
uint32_t sub_800644F4(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_800644FC(RRJMemory *m, uint32_t manager, RRJRaceLeafCall call);
uint32_t sub_8006396C(RRJMemory *m);
uint32_t sub_80063C5C(RRJMemory *m, uint32_t player_index);
uint32_t sub_8002C4F8(RRJMemory *m, uint32_t position, uint32_t link, uint32_t player_index);
uint32_t sub_80021988(RRJMemory *m, int32_t minimum, int32_t maximum,
                      RRJRaceLeafCall call);
uint32_t sub_8002AF80(RRJMemory *m, uint32_t matrix);
uint32_t sub_8002BAE8(RRJMemory *m, uint32_t first_value, uint32_t second_value,
                      uint32_t ordering_slot);
uint32_t sub_8002BE14(RRJMemory *m, uint32_t point, uint32_t ordering_slot,
                      uint32_t player_index);
uint32_t sub_8001034C(RRJMemory *m, uint32_t matrix, uint32_t count, uint32_t vertices,
                      uint32_t records, uint32_t screens, uint32_t clips);
uint32_t sub_800104C8(RRJMemory *m, uint32_t matrix, uint32_t count, uint32_t vertices,
                      uint32_t records, uint32_t screens);
uint32_t sub_80068D50(RRJMemory *m, uint32_t object, uint32_t entry_index);
uint32_t sub_80068E2C(RRJMemory *m, uint32_t object, uint32_t entry_index);
uint32_t sub_80068EB8(RRJMemory *m, uint32_t object);
uint32_t sub_80069200(RRJMemory *m, uint32_t object);
uint32_t sub_80035958(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_8006929C(RRJMemory *m, uint32_t base_index, uint32_t packed_indices, uint32_t depth_shift);
uint32_t sub_80069784(RRJMemory *m, uint32_t base_index, uint32_t packed_indices, uint32_t depth_shift);
uint32_t sub_80069CF0(RRJMemory *m, uint32_t base_index, uint32_t packed_indices, uint32_t depth_shift);
uint32_t sub_8006A25C(RRJMemory *m, uint32_t base_index, uint32_t packed_indices, uint32_t packet_color);
uint32_t sub_8006A630(RRJMemory *m, uint32_t object, uint32_t group_index);
uint32_t sub_8006C888(RRJMemory *m, uint32_t object, uint32_t group_index);
uint32_t sub_8006D350(RRJMemory *m, uint32_t object, uint32_t group_index);
uint32_t sub_8006DC20(RRJMemory *m, uint32_t object, uint32_t group_index);
uint32_t sub_8006E474(RRJMemory *m, uint32_t object, uint32_t group_index);
uint32_t sub_8006F5D0(RRJMemory *m, uint32_t object, uint32_t group_index);
uint32_t sub_80068FCC(RRJMemory *m, uint32_t object);
uint32_t sub_800706A4(RRJMemory *m, uint32_t object);
int32_t sub_800669E8(RRJMemory *m, uint32_t object, uint32_t output);
uint32_t sub_800220A4(RRJMemory *m, uint32_t matrix, uint32_t count, uint32_t vertices,
                      uint32_t records, uint32_t screens, uint32_t clips);
uint32_t sub_80030B7C(RRJMemory *m, uint32_t first, uint32_t second);
uint32_t sub_80030C8C(RRJMemory *m, uint32_t index);
void sub_80030CB8(RRJMemory *m, int32_t value);
uint32_t sub_80022484(RRJMemory *m, uint32_t page, uint32_t mode, uint32_t first_output,
                      uint32_t second_output, uint32_t table_index);
uint32_t sub_800225E0(RRJMemory *m, uint32_t page, uint32_t mode, uint32_t table_index);
uint32_t sub_80033634(RRJMemory *m, int32_t index, uint32_t mode, uint32_t group);
uint32_t sub_800309E4(RRJMemory *m, uint32_t first_value, uint32_t second_value,
                      uint32_t first_record, uint32_t second_record, uint32_t kind,
                      RRJRaceLeafCall call);
uint32_t sub_80030BCC(RRJMemory *m, uint32_t value_output, uint32_t record_output,
                      uint32_t kind, RRJRaceLeafCall call);
int32_t sub_800348CC(RRJMemory *m, uint32_t record, uint32_t mode);
uint32_t sub_80033304(RRJMemory *m, uint32_t owner, uint32_t mode, uint32_t group);
uint32_t sub_800326BC(RRJMemory *m, int32_t key, uint32_t group);
int32_t sub_80034D38(RRJMemory *m, int32_t key, uint32_t unused, uint32_t group);
int32_t sub_80033770(RRJMemory *m, uint32_t page, uint32_t mode, uint32_t record_index,
                     uint32_t group, RRJRaceLeafCall call);
int32_t sub_800339F8(RRJMemory *m, uint32_t mode, uint32_t group, RRJRaceLeafCall call);
uint32_t sub_8003367C(RRJMemory *m, uint32_t owner, uint32_t indices, int32_t count,
                      uint32_t mode);
int32_t sub_8003348C(RRJMemory *m, uint32_t mode, uint32_t group);
int32_t sub_80032F78(RRJMemory *m, uint32_t object, uint32_t mode, uint32_t group,
                     RRJRaceLeafCall call);
uint32_t sub_80032D24(RRJMemory *m, uint32_t group, RRJRaceLeafCall call);
uint32_t sub_80066A84(RRJMemory *m, uint32_t object);
int32_t sub_8001FD24(RRJMemory *m, uint32_t first, uint32_t second, uint32_t third,
                     uint32_t output);
uint32_t sub_80066B98(RRJMemory *m, uint32_t object, uint32_t child_index);
uint32_t sub_80067064(RRJMemory *m, uint32_t mode, uint32_t object);
uint32_t sub_80066B28(RRJMemory *m, uint32_t object);
uint32_t sub_8006745C(RRJMemory *m, uint32_t object);
uint32_t sub_80066ECC(RRJMemory *m, uint32_t object);
uint32_t sub_80066EC4(RRJMemory *m, uint32_t object);
uint32_t sub_80029048(RRJMemory *m, uint32_t output, uint32_t center, uint32_t angle,
                      int32_t radius);
uint32_t sub_8002990C(RRJMemory *m, uint32_t object, uint32_t output);
int32_t sub_80029CA4(RRJMemory *m, uint32_t object);
uint32_t sub_80028534(RRJMemory *m, uint32_t object);
uint32_t sub_80027B80(RRJMemory *m, uint32_t object);
uint32_t sub_80026960(RRJMemory *m, uint32_t object, uint32_t player_index,
                      uint32_t position, uint32_t direction);
uint32_t sub_80025EE0(RRJMemory *m, uint32_t object, uint32_t player_index,
                      uint32_t position, uint32_t direction);
uint32_t sub_800251E4(RRJMemory *m, uint32_t object, uint32_t player_index);
uint32_t sub_80068468(RRJMemory *m, uint32_t object, uint32_t player_index,
                      RRJRaceLeafCall call);
uint32_t sub_8006780C(RRJMemory *m, uint32_t object, uint32_t player_index,
                      RRJRaceLeafCall call);
uint32_t sub_80067690(RRJMemory *m, uint32_t owner, uint32_t player_index,
                      RRJRaceLeafCall call);
uint32_t sub_80028E8C(RRJMemory *m, uint32_t position, uint32_t output,
                      uint32_t player_index);
uint32_t sub_80029174(RRJMemory *m, uint32_t vertices, uint32_t position,
                      uint32_t center, uint32_t player_index);
uint32_t sub_8002926C(RRJMemory *m, uint32_t vertices, uint32_t flags,
                      uint32_t texture_index, uint32_t clut_index, uint32_t command);
uint32_t sub_800295AC(RRJMemory *m, uint32_t object, uint32_t vertices,
                      uint32_t flags, uint32_t texture_index, uint32_t uv_variant);
uint32_t sub_8002AB14(RRJMemory *m, uint32_t object, uint32_t record);
uint32_t sub_8002A2C8(RRJMemory *m, uint32_t object, uint32_t record,
                      uint32_t age, uint32_t player_index, uint32_t output_flags);
uint32_t sub_80028E74(RRJMemory *m, uint32_t record);
uint32_t sub_800289E8(RRJMemory *m, uint32_t object, uint32_t scale,
                      uint32_t output);
uint32_t sub_80028C78(RRJMemory *m, uint32_t object, uint32_t output,
                      uint32_t input, uint32_t player_index);
uint32_t sub_8002A8E4(RRJMemory *m, uint32_t age, uint32_t record);
uint32_t sub_80017F64(RRJMemory *m, uint32_t actor_id, uint32_t listener,
                      RRJRaceLeafCall call);
uint32_t sub_800182B0(RRJMemory *m, uint32_t listener, RRJRaceLeafCall call);
uint32_t sub_8001836C(RRJMemory *m, uint32_t listener, RRJRaceLeafCall call);
uint32_t sub_8002A974(RRJMemory *m, uint32_t object, uint32_t record,
                      uint32_t age, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_80029D88(RRJMemory *m, uint32_t object, uint32_t record,
                      uint32_t player_index, uint32_t age, RRJRaceLeafCall call);
uint32_t sub_8002823C(RRJMemory *m, uint32_t object, uint32_t player_index,
                      RRJRaceLeafCall call);
uint32_t sub_80067770(RRJMemory *m, uint32_t owner, uint32_t player_index,
                      RRJRaceLeafCall call);
uint32_t sub_800674D4(RRJMemory *m, uint32_t indices, int32_t count,
                      uint32_t player_index, RRJRaceLeafCall call);
int32_t sub_8003328C(RRJMemory *m, uint32_t owner, int32_t group);
uint32_t sub_800CAAF0(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call);
uint32_t sub_800CB02C(RRJMemory *m, uint32_t actor);
uint32_t sub_80023A14(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_80023DB8(RRJMemory *m, uint32_t output);
uint32_t sub_80023FBC(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_80023CAC(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_80030E58(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_80033198(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_80023900(RRJMemory *m, uint32_t type, uint32_t payload, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_800243EC(RRJMemory *m, uint32_t type, uint32_t ranges);
uint32_t sub_800318E8(RRJMemory *m, uint32_t record, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_800312D0(RRJMemory *m, uint32_t player_index);
uint32_t sub_80018E54(RRJMemory *m, uint32_t group, RRJRaceLeafCall call);
uint32_t sub_80043E24(RRJMemory *m);
uint32_t sub_80043DC4(RRJMemory *m, uint32_t stack);
uint32_t sub_800C89A0(RRJMemory *m);
uint32_t sub_8002F2E8(RRJMemory *m, uint32_t argument, RRJRaceLeafCall call);
uint32_t sub_8002F17C(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_80070F9C(RRJMemory *m, uint32_t source, uint32_t output);
uint32_t sub_800674C8(RRJMemory *m);
uint32_t sub_8008D56C(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_80084E10(RRJMemory *m, uint32_t actor, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_80067AC4(RRJMemory *m, uint32_t actor, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_800358C0(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_80035F48(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_800363F0(RRJMemory *m, uint32_t record);
uint32_t sub_80035040(RRJMemory *m, uint32_t record, uint32_t vertices, int32_t count);
uint32_t sub_800351EC(RRJMemory *m, uint32_t record, uint32_t vertices, int32_t count,
                      uint32_t average_output, uint32_t minimum_output, uint32_t maximum_output);
uint32_t sub_800353C4(RRJMemory *m, uint32_t count_pointer, int32_t count, uint32_t indices,
                      uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_80036438(RRJMemory *m, int32_t count, uint32_t indices, uint32_t player_index);
uint32_t sub_80036614(RRJMemory *m, uint32_t record, uint32_t slot, uint32_t flags);
uint32_t sub_80035680(RRJMemory *m, int32_t count, uint32_t indices, uint32_t player_index);
uint32_t sub_800106DC(RRJMemory *m, uint32_t input, uint32_t output, uint32_t matrix);
uint32_t sub_8001064C(RRJMemory *m);
uint32_t sub_80013828(RRJMemory *m, uint32_t entry);
uint32_t sub_80023868(RRJMemory *m, uint32_t unused);

#endif
