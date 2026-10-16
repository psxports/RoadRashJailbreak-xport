#ifndef RRJ_GAME_MAIN_H
#define RRJ_GAME_MAIN_H

#include <stdint.h>

uint32_t sub_800A966C(uint32_t actor, const uint16_t normal[3], int32_t minimum);
uint32_t sub_800B2844(uint32_t actor, uint32_t candidate);
uint32_t sub_800A9868(uint32_t actor, const uint16_t normal[3], int32_t impact_speed, const uint16_t other_direction[3], uint32_t other_identity, uint32_t force);

uint32_t sub_800B675C(const uint32_t box[24], const uint16_t basis[9], uint32_t face, uint16_t normal[3], uint32_t corner[3]);
uint32_t sub_800B7030(const uint32_t points[24], const uint16_t basis[9], const uint32_t box[24], uint32_t excluded_faces, uint32_t *face, uint32_t *depth);
uint32_t sub_800AA34C(const uint32_t points[24], const uint8_t basis[32], uint32_t bounds[4]);
const uint8_t *sub_800AA140(uint32_t actor, const uint8_t *other, uint32_t *actor_face, uint32_t *other_face, uint32_t translation[3]);

uint32_t sub_800ABE78(uint32_t actor, uint32_t other, uint32_t *linked, uint32_t *angle, uint32_t *lateral, uint32_t *longitudinal);
uint32_t sub_800B5B48(uint32_t actor, uint32_t other, uint16_t output[3]);

uint32_t sub_800AB7A0(uint32_t actor, uint32_t other, uint32_t propagated);
uint32_t sub_800A7AB4(uint32_t actor, const uint32_t delta[3], uint32_t excluded);
uint32_t sub_800A7ABC(uint32_t actor, const uint32_t delta[3], uint32_t excluded, uint32_t initial_count);
uint32_t sub_800AC56C(uint32_t alignment, uint32_t first_side, uint32_t other_mode);
uint32_t sub_800AA474(uint32_t actor, uint32_t other, uint32_t linked, uint32_t angle, uint32_t lateral, uint32_t longitudinal, uint32_t *actor_face, uint32_t *other_face, uint32_t translation[3]);
uint8_t *sub_800AAD30(uint32_t actor, uint8_t *other, uint32_t *actor_face, uint32_t *other_face, uint32_t translation[3], uint32_t *power);
uint32_t sub_800AC130(uint32_t actor, uint32_t other, uint32_t actor_face, uint32_t other_face);

uint32_t sub_800B71AC(const uint32_t points[24], const uint16_t direction[3], int32_t distance, const uint32_t box[24], const uint16_t basis[9], uint32_t excluded_faces, uint32_t *face, uint32_t *depth);
uint32_t sub_80081D7C(uint32_t actor, uint32_t other, uint32_t actor_face, uint32_t other_face, uint16_t normal[3]);

uint32_t sub_800B7810(const uint32_t points[24], const uint16_t basis[9], uint32_t face, uint32_t output[12], uint16_t planes[12]);

uint32_t sub_800B6D70(const uint32_t point[3], const uint32_t vertices[], const uint16_t planes[], int32_t count, uint32_t tolerance);
uint32_t sub_800B74F0(const uint32_t points[24], const uint16_t direction[3], int32_t distance, const uint32_t box[24], const uint16_t basis[9], uint32_t excluded_faces, uint32_t *face, uint32_t *depth);

uint32_t sub_80071F0C(uint32_t mode, uint32_t game_mode);

uint32_t sub_8007ED34(uint32_t enabled);

uint32_t sub_80078F44(uint32_t descriptor, uint32_t id);
uint32_t sub_80075BD0(uint32_t menu, uint32_t entry, uint32_t data);

uint32_t sub_80075BE4(uint32_t actor, int32_t delta);

uint32_t sub_800A3A84(uint32_t actor);

uint32_t sub_8008F404(uint32_t actor, int32_t delta);
uint32_t sub_800976C4(uint32_t actor, int32_t delta);
uint32_t sub_80097BCC(uint32_t actor, int32_t delta);

uint32_t sub_8007EA64(uint32_t actor);

uint32_t sub_8008F754(uint32_t actor, int32_t delta, uint32_t blend);
uint32_t sub_8008FD5C(uint32_t actor);
uint32_t sub_8009926C(uint32_t actor, int32_t delta);
uint32_t sub_800986D0(uint32_t actor);
uint32_t sub_8009A038(uint32_t actor);

uint32_t sub_800C341C(uint32_t actor);
uint32_t sub_800C3630(uint32_t actor);
uint32_t sub_800C31CC(uint32_t actor);

uint32_t sub_800714A0(uint16_t output[9], const uint16_t quaternion[4]);
uint32_t sub_8007198C(const uint16_t quaternion[4], uint16_t output[9]);
uint32_t sub_80071A28(uint16_t quaternion[4]);
uint32_t sub_800A40D4(uint32_t actor);
uint32_t sub_800A41EC(void);

uint32_t sub_8007F20C(uint32_t slot, int32_t value);
uint32_t sub_8007FDA8(void);
uint32_t sub_800806D0(void);
uint32_t sub_800809A8(uint32_t id);
uint32_t sub_800809E0(int16_t id);
uint32_t sub_80080CC8(void);

uint32_t sub_80065B10(void);
uint32_t sub_80063720(void);

uint32_t sub_800665E8(void);
uint32_t sub_80068E88(void);
uint32_t sub_8006D174(void);

uint32_t sub_800805C8(void);

uint32_t sub_8007A5EC(void);
uint32_t sub_80076228(void);
uint32_t sub_80062438(void);
uint32_t sub_80060D0C(void);
uint32_t sub_800621E0(uint32_t sectors, uint32_t buffer_bytes, uint32_t descriptors);

uint32_t sub_80080544(void);

uint32_t sub_80080C50(void);
uint32_t sub_80063BFC(void);
uint32_t sub_80065C38(void);
uint32_t sub_80078EF4(void);
uint32_t sub_8005FB1C(void);
uint32_t sub_80080C04(void);
uint32_t sub_80080220(void);
uint32_t sub_8006460C(uint32_t resource);
uint32_t sub_800647E4(uint32_t resource);
uint32_t sub_8006B800(uint32_t menu, uint32_t resource, int32_t first, int32_t end);
uint32_t sub_8006B7BC(uint32_t menu);
uint32_t sub_80069440(uint32_t menu);
uint32_t sub_80069560(uint32_t menu);
uint32_t sub_8006AAF8(uint32_t menu);
uint32_t sub_8006AC80(uint32_t menu);
uint32_t sub_8006A838(uint32_t menu);
uint32_t sub_8006AB58(uint32_t menu);
uint32_t sub_8006AD20(uint32_t menu);
uint32_t sub_800B208C(uint32_t actor);
uint32_t sub_800AE794(uint32_t actor, uint32_t surface);
uint32_t sub_800B09C4(uint32_t actor, uint32_t surface);
uint32_t sub_800ADC74(uint32_t actor, uint32_t surface, int32_t radius, uint32_t translation[3], uint32_t *power);
uint32_t sub_800AD04C(uint32_t actor, uint32_t other);
uint32_t sub_800C29F0(uint32_t rider, uint32_t actor);
uint32_t sub_800AF0A0(uint32_t actor, uint32_t surface, uint16_t normal[3], uint32_t mode, uint32_t force, uint32_t translation[3]);
uint32_t sub_800A77B0(void);
uint32_t sub_800B1978(uint32_t actor);
uint32_t sub_8006C354(void);
uint32_t sub_8006E008(void);

uint32_t sub_8007F304(void);
uint32_t sub_8007F2A8(int32_t track);
uint32_t sub_8007F37C(void);

uint32_t sub_8007FEDC(void);

uint32_t sub_80063908(void);
uint32_t sub_8007FD20(void);
uint32_t sub_8007E7BC(int32_t track, int32_t value);

uint32_t sub_80068688(uint32_t id);
uint32_t sub_8007E770(int32_t first, int32_t last);
uint32_t sub_8007E7EC(int32_t track);
uint32_t sub_8007E2C8(void);

uint32_t sub_8007E724(int32_t first, int32_t last);
uint32_t sub_8007DDA8(void);

void sub_80064610(void);

uint32_t sub_80063C00(void);
uint32_t sub_80063C20(void);
uint32_t sub_80063E48(void);

void sub_8005D960(void);
void sub_8005D968(void);
uint32_t sub_8005D988(void);
uint32_t sub_8005D5E4(void);
uint32_t sub_8005D018(void);
uint32_t sub_8005BE40(void);

uint32_t sub_8005D338(void);
uint32_t sub_8005D38C(void);
uint32_t sub_8005D3F4(void);
uint32_t sub_8005D410(void);
uint32_t sub_8005D500(void);
void sub_8005D330(void);
uint32_t sub_8005EBDC(void);
uint32_t sub_8005ED94(void);
uint32_t sub_8005D568(uint32_t count);

uint32_t sub_8005E930(void);
uint32_t sub_8005E4E8(uint32_t players);
uint32_t sub_8005E6CC(const char *path, uint32_t players);
uint32_t sub_8005D9EC(void);

uint32_t sub_8005E9A0(uint32_t players);
uint32_t sub_8005D8D0(void);
uint32_t sub_80063500(void);

uint32_t sub_80063D94(const char *name);
uint32_t sub_800655D4(const char *path);
uint32_t sub_800674F8(void);

uint32_t sub_80061994(void);
uint32_t sub_800611C4(void);
uint32_t sub_80062384(uint32_t header, uint32_t data);
uint32_t sub_80062430(uint32_t header, uint32_t data);
uint32_t sub_8006250C(uint32_t header, uint32_t data);
uint32_t sub_800626E4(uint32_t header, uint32_t data);
uint32_t sub_800627A8(uint32_t header, uint32_t data);

uint32_t sub_80061100(void);
uint32_t sub_80060F58(uint32_t data, uint32_t players);
uint32_t sub_80061D18(uint32_t header, uint32_t data);
uint32_t sub_80061FAC(uint32_t header, uint32_t data);

void sub_8005BDAC(uint32_t data);
uint32_t sub_8005DA38(uint32_t data);
uint32_t sub_8005DAD0(uint32_t data, uint32_t index);
uint32_t sub_8005DBB8(uint32_t data);

uint32_t sub_8005DDB8(uint32_t data);
uint32_t sub_8005C920(uint32_t data, int32_t bytes);
uint32_t sub_8006270C(uint32_t header, uint32_t data);
uint32_t sub_80062750(uint32_t header, uint32_t data);
void sub_80061C24(uint32_t header, uint32_t data);
uint32_t sub_800619A0(void);

int32_t sub_8005C010(uint32_t identity);
uint32_t sub_8005C054(uint32_t index);
int32_t sub_8005BD80(uint32_t kind, uint32_t index);
int32_t sub_8005CB9C(uint32_t identity, uint32_t data);
int32_t sub_8005CC4C(uint32_t identity, uint32_t data, uint32_t part);
uint32_t sub_8005CE5C(uint32_t data);
int32_t sub_8005CD60(uint32_t identity, uint32_t data, uint32_t part, uint32_t piece);
int32_t sub_8005CE78(uint32_t identity, uint32_t data, uint32_t part);
int32_t sub_8005C0C4(uint32_t *identity_out, uint32_t data, int32_t bytes, uint32_t retain);
int32_t sub_8005CA10(const char *geometry_path, const char *texture_path);

uint32_t sub_80064034(uint32_t kind, uint32_t rider);
int32_t sub_8005C30C(const char *path, uint32_t *bytes_out);
int32_t sub_8005C45C(int32_t level);
uint32_t sub_8005C630(int32_t level);

uint32_t sub_80061258(uint32_t image, uint16_t output[16]);
uint32_t sub_80061430(int32_t slot);
int32_t sub_80061528(const char *name, uint32_t upload, uint32_t split);

uint32_t sub_80060C78(uint32_t players);
uint32_t sub_80060EFC(uint32_t players);
uint32_t sub_80060894(uint32_t packet, uint32_t depth, const uint32_t vector[4], uint32_t clut_x, uint32_t clut_y);
uint32_t sub_80060AA8(void);

uint32_t sub_80063448(uint32_t player);

void sub_80013AE4(void);
void sub_800609B0(void);
uint32_t sub_80060BE8(uint32_t players);
uint32_t sub_8006068C(void);

int32_t sub_80062D84(uint32_t handle, const uint32_t bank[3], const uint32_t *buffer_pointer, uint32_t scratch);
uint32_t sub_800627F8(void);

uint32_t sub_8005F104(uint32_t data, uint16_t output[5]);
uint32_t sub_8005F1E4(const char *path);
uint32_t sub_8005EF48(void);
uint32_t sub_8005F9EC(uint32_t palette, int32_t index);
uint32_t sub_8005EFFC(uint32_t packet, uint32_t palette);
uint32_t sub_8005FA88(void);
uint32_t sub_800635D0(void);

uint32_t sub_8005D130(uint32_t owner);
uint32_t sub_8005D1A0(uint32_t owner, int32_t count);
uint32_t sub_800653E8(const char *name);
uint32_t sub_8005C7F0(int32_t selection);
uint32_t sub_8005CEE0(uint32_t player);
int32_t sub_8006AD4C(void);

uint32_t sub_8006ACD0(uint32_t route, uint32_t distance, uint32_t offset);

uint32_t sub_80068470(const uint8_t data[280]);
uint32_t sub_8006AEF4(void);

uint8_t *sub_80064084(uint8_t *output, int32_t limit, uint32_t source);
const uint8_t *sub_80064144(const uint8_t *source, int32_t *value_out);
const uint8_t *sub_800643B4(const uint8_t *source, uint8_t *output);
const uint8_t *sub_80069B10(const uint8_t *guard, const uint8_t *source, uint32_t delimiter, int32_t *value_out);
int32_t sub_80069B8C(uint32_t data, int32_t bytes);
uint32_t sub_80069C60(uint32_t data, int32_t bytes, int32_t race, uint32_t *data_out, int32_t *bytes_out);
void sub_80069DEC(uint32_t data, int32_t bytes);
uint32_t sub_8006A014(uint32_t entry);

uint32_t sub_8006A7C0(void);
uint32_t sub_8006A98C(void);
uint32_t sub_8006A9F4(uint32_t data, uint32_t bytes);
uint32_t sub_8006ABC8(const char *prefix);
uint32_t sub_8006AC4C(const char *prefix);

void sub_8006A0C8(uint32_t data, int32_t bytes);
uint32_t sub_8006A7F8(const char *prefix);
uint32_t sub_8006AC6C(const char *prefix);

uint32_t sub_80063670(void);
void sub_8006982C(void);

uint32_t sub_80069000(void);
uint32_t sub_8006968C(void);
uint32_t sub_800645E4(const uint8_t *data, int32_t offset);

uint32_t sub_80068D54(void);
uint32_t sub_80068AA4(void);
uint32_t sub_80067784(void);
uint32_t sub_80068500(void);
uint32_t sub_80069394(void);
uint32_t sub_80068614(uint32_t kind, uint32_t *attached_out);
uint32_t sub_80068684(void);
uint32_t sub_800689D0(const uint8_t data[280]);

uint32_t sub_80065468(void);
uint32_t sub_800654BC(uint32_t buffer);
uint32_t sub_80064B44(uint8_t *buffer, int32_t capacity);
uint32_t sub_80067B00(const uint8_t *data, int32_t bytes);
uint32_t sub_80065A94(uint32_t record[3], const uint8_t *resources, uint32_t selection, uint32_t *mask_out, uint32_t flags);
uint32_t sub_800650A0(uint32_t actor);
uint32_t sub_800670FC(uint32_t owner, const uint8_t *resources, uint32_t variant, int32_t slot);

uint32_t sub_80065974(int32_t slot);
uint32_t sub_80065A04(int32_t slot);
uint32_t sub_8005DD9C(uint32_t player);
uint32_t sub_80064C0C(const uint8_t *resources, uint32_t variant, uint32_t actor);

uint32_t sub_80065558(const char *path, uint8_t data[448]);
uint32_t sub_80065648(void);
uint32_t sub_800656A0(uint32_t actor, uint32_t piece, uint32_t direction);
uint32_t sub_8006581C(uint32_t actor);

uint32_t *sub_8006AD24(uint32_t output[8]);
uint32_t sub_800644C8(uint32_t actor);
uint32_t sub_80063158(void);

uint32_t sub_80069618(const char *path);
uint32_t sub_80067564(void);
uint32_t sub_800696B8(void);
uint32_t sub_80068740(void);

uint32_t sub_80066E1C(uint32_t actor);

uint32_t sub_8005BB50(uint32_t chunk);
uint32_t sub_8005BAF8(uint32_t chunk);
uint32_t sub_8005BB6C(uint32_t entry, uint32_t data, uint32_t bytes);
uint32_t sub_8005BCDC(uint32_t owner, const char *path);

uint32_t sub_80060028(uint32_t packet, const uint8_t color[3]);
uint32_t sub_8005FE24(void);
uint32_t sub_8005ED4C(uint32_t value);
uint32_t sub_8005EC20(void);
uint32_t sub_80060088(void);
uint32_t sub_80063B90(void);

uint32_t sub_8006A944(void);
uint32_t sub_8006A9B8(void);
uint32_t sub_8006ACB0(void);
uint32_t sub_8006A8D0(void);
uint32_t sub_8006B3FC(void);
uint32_t sub_800686B8(void);
uint32_t sub_8005D618(uint32_t object);
uint32_t sub_800694B0(void);
void sub_8006860C(void);
void sub_80068A50(void);
uint32_t sub_800677EC(void);
uint32_t sub_800696AC(void);
uint32_t sub_80069238(void);
uint32_t sub_80069AC8(void);

uint32_t sub_8005D520(void);
void sub_8005D010(void);
uint32_t sub_8005BA9C(uint32_t entry);
uint32_t sub_8005BCA0(uint32_t owner, uint32_t entry);
uint32_t sub_8005D274(uint32_t owner);
uint32_t sub_8005BF2C(uint32_t slot);
uint32_t sub_8005BEC0(void);
uint32_t sub_8005D5C4(uint32_t data);
uint32_t sub_8005D920(void);

uint32_t sub_8005F0B8(void);
uint32_t sub_800606FC(void);
uint32_t sub_80060754(void);
uint32_t sub_80063D24(void);
uint32_t sub_800632B4(uint32_t player);
uint32_t sub_8006326C(void);
uint32_t sub_8006335C(void);
uint32_t sub_80062E9C(void);
void sub_8005D610(void);
void sub_8005DA30(void);
uint32_t sub_80063A20(void);

void sub_800B2AF8(void);
uint32_t sub_800B2C98(uint32_t actor, uint32_t other);
uint32_t sub_800B2D44(uint32_t actor, uint32_t other);
uint32_t sub_800B2D88(uint32_t bounds, uint32_t actor);

uint32_t sub_800B2B00(uint32_t actor, uint32_t bounds);

uint32_t sub_800A91AC(uint32_t actor_face, uint32_t other_face, uint32_t actor, uint32_t other, uint32_t translation[3]);

uint32_t sub_800B6B58(const uint32_t point[3], const uint16_t normal[3], const uint32_t origin[3]);

uint32_t sub_800AF224(uint32_t actor, uint32_t surface, uint32_t actor_face, uint32_t surface_face, uint32_t translation[3]);
uint32_t sub_800AC958(uint32_t actor, uint32_t other, uint32_t actor_face, uint32_t other_face, uint32_t translation[3]);
uint32_t sub_800AC5BC(uint32_t actor, uint32_t other);

uint32_t sub_800B11B4(uint32_t actor, uint32_t surface, uint32_t actor_face, uint32_t surface_face, uint32_t translation[3], uint16_t normal[3]);
uint32_t sub_800B0D8C(uint32_t actor, uint32_t surface);


uint32_t sub_800C6358(uint32_t order[3]);
uint32_t sub_800C63CC(uint16_t rect[4]);
void *sub_80048FBC(void *environment);
uint32_t sub_800C5918(void);
uint32_t sub_800C64F4(uint32_t order[3], uint32_t position, uint32_t dimensions);
uint32_t sub_800C67FC(uint32_t position, uint32_t dimensions);
uint32_t sub_800C6FAC(void);
uint32_t sub_800C79D4(uint32_t order[3], uint32_t position, uint32_t dimensions);
uint32_t sub_800C65A0(uint32_t order[3], uint32_t position, uint32_t dimensions);


uint32_t sub_800C6634(uint32_t order[3], uint32_t position, uint32_t dimensions);
uint32_t sub_800C84C0(uint32_t actor, uint32_t player);
uint32_t sub_800C84F8(uint32_t position, uint32_t dimensions, uint32_t mode);
uint32_t sub_800C6FF8(void);
uint32_t sub_800C7510(void);
uint32_t sub_800C5B5C(uint32_t position, uint32_t dimensions);
uint32_t sub_800C5C68(uint32_t position, uint32_t dimensions, uint32_t actor, uint32_t color);
uint32_t sub_800C5DF0(uint32_t position, uint32_t dimensions, uint32_t actor);

uint32_t sub_800C83D8(int32_t time, uint32_t mode, uint32_t position, uint32_t dimensions, uint32_t color);

uint32_t sub_800C836C(int32_t value, uint32_t mode, uint32_t position, uint32_t dimensions);

uint32_t sub_8007FF4C(void);
uint32_t sub_8007BB34(void);

void sub_800403CC(void);
void sub_80063FA0(void);

uint32_t sub_80072AF4(uint32_t menu, uint32_t entry);
uint32_t sub_80072C44(uint32_t menu, uint32_t entry);
uint32_t sub_80072D94(uint32_t menu, uint32_t entry);
uint32_t sub_80072F2C(uint32_t menu, uint32_t entry);
uint32_t sub_8007593C(uint32_t menu, uint32_t entry);
uint32_t sub_8007219C(uint32_t menu, uint32_t entry);
uint32_t sub_80072248(uint32_t menu, uint32_t entry);
uint32_t sub_80072388(uint32_t menu, uint32_t entry);
uint32_t sub_800724D0(uint32_t menu, uint32_t entry);
uint32_t sub_80072668(uint32_t menu, uint32_t entry);
uint32_t sub_800727C8(uint32_t menu, uint32_t entry);
uint32_t sub_80072910(uint32_t menu, uint32_t entry);
uint32_t sub_800733BC(uint32_t menu, uint32_t entry);
uint32_t sub_80073BAC(uint32_t menu, uint32_t entry);
uint32_t sub_80074194(uint32_t menu, uint32_t entry);
uint32_t sub_80074554(uint32_t menu, uint32_t entry);
uint32_t sub_80075D0C(uint32_t menu, uint32_t entry);
uint32_t sub_80075098(uint32_t menu, uint32_t entry);
uint32_t sub_80075AFC(uint32_t menu, uint32_t entry);
uint32_t sub_80073074(uint32_t menu, uint32_t entry, uint32_t selector);
uint32_t sub_80073234(uint32_t menu, uint32_t entry, uint32_t selector);
uint32_t sub_800758D4(uint32_t menu, uint32_t entry);
uint32_t sub_80074C9C(void);
uint32_t sub_80071064(uint32_t menu, uint32_t rectangle, int32_t id);
uint32_t sub_80071AF0(uint32_t rectangle, int32_t id);
uint32_t sub_80070DD4(uint32_t menu, uint32_t rectangle);
uint32_t sub_80072A60(int32_t player);
uint32_t sub_80072A80(int32_t player, int32_t option);

uint32_t sub_80071FDC(int32_t rider);
uint32_t sub_800720B4(int32_t track);

uint32_t sub_800642DC(uint32_t list);
uint32_t sub_80064474(uint32_t list);

uint32_t sub_80071F70(uint32_t mode);

uint32_t sub_8006EE20(const uint8_t rectangle[16], uint32_t link);
uint32_t sub_8006ECDC(uint32_t menu, const uint8_t frame[34], uint32_t link);
uint32_t sub_8006EDE4(uint32_t menu, uint32_t entry);

#endif
