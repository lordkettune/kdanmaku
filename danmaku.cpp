#include "danmaku.h"
#include "pattern.h"
#include "hitbox.h"

#include "servers/visual_server.h"

#define MAX_SHOT_SPRITES 32

void Danmaku::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            VS::get_singleton()->canvas_item_set_update_when_visible(get_canvas_item(), true);
            VS::get_singleton()->connect("frame_pre_draw", this, "_update_buffer");
            set_physics_process(true);
        } break;

        case NOTIFICATION_EXIT_TREE: {
            VS::get_singleton()->canvas_item_set_update_when_visible(get_canvas_item(), false);
            VS::get_singleton()->disconnect("frame_pre_draw", this, "_update_buffer");
            set_physics_process(false);
        } break;

        case NOTIFICATION_DRAW: {
            RID atlas_rid;
            if (atlas.is_valid()) {
                atlas_rid = atlas->get_rid();
            }
            VS::get_singleton()->canvas_item_add_multimesh(get_canvas_item(), multimesh, atlas_rid);
        } break;
    }
}

void Danmaku::add_pattern(Pattern* p_pattern) {
    patterns.push_back(p_pattern);
}

void Danmaku::remove_pattern(Pattern* p_pattern) {
    patterns.erase(p_pattern);
}

void Danmaku::add_hitbox(Hitbox* p_hitbox) {
    hitbox = p_hitbox;
}

void Danmaku::remove_hitbox() {
    hitbox = NULL;
}

Hitbox* Danmaku::get_hitbox() const {
    return hitbox;
}

Shot* Danmaku::capture() {
    Shot* shot = free_shots[free_shots.size() - 1];
    free_shots.remove(free_shots.size() - 1);
    return shot;
}

void Danmaku::release(Shot* p_shot) {
    free_shots.push_back(p_shot);
}

Ref<ShotSprite> Danmaku::get_sprite(const String& p_key) const {
    for (int i = 0; i != sprites.size(); ++i) {
        if (sprites[i]->get_key() == p_key)
            return sprites[i];
    }
    ERR_FAIL_V_MSG(Ref<ShotSprite>(), "Failed to find shot sprite");
}

void Danmaku::clear_circle(Vector2 p_origin, float p_radius) {
    for (int i = 0; i != patterns.size(); ++i) {
        Transform2D transform = patterns[i]->get_global_transform();
        patterns[i]->clear([=](Shot* shot) {
            return (transform.xform(shot->get_position()) - p_origin).length() <= p_radius;
        });
    }
}

void Danmaku::clear_rect(Rect2 p_rect) {
    for (int i = 0; i != patterns.size(); ++i) {
        Transform2D transform = patterns[i]->get_global_transform();
        patterns[i]->clear([=](Shot* shot) {
            return p_rect.has_point(transform.xform(shot->get_position()));
        });
    }
}

void Danmaku::set_max_shots(int p_max_shots) {
    ERR_FAIL_COND_MSG(p_max_shots < 1, "Danmaku must have at least one shot!");
    if (patterns.size() != 0) {
        WARN_PRINT("Changing max_shots while patterns exist!");
    }
    _destroy();

    max_shots = p_max_shots;

    free_shots.resize(max_shots);
    for (int i = 0; i != max_shots; ++i) {
        free_shots.write[i] = memnew(Shot);
    }

    VS::get_singleton()->multimesh_allocate(multimesh, max_shots, VS::MULTIMESH_TRANSFORM_2D, VS::MULTIMESH_COLOR_NONE, VS::MULTIMESH_CUSTOM_DATA_FLOAT);
    buffer.resize((8 + 4) * max_shots);
}

int Danmaku::get_max_shots() const {
    return max_shots;
}

int Danmaku::get_free_shot_count() const {
    return free_shots.size();
}

int Danmaku::get_active_shot_count() const {
    return max_shots - free_shots.size();
}

int Danmaku::get_pattern_count() const {
    return patterns.size();
}

void Danmaku::set_region(const Rect2& p_region) {
    region = p_region;
}

Rect2 Danmaku::get_region() const {
    return region;
}

void Danmaku::set_tolerance(float p_tolerance) {
    tolerance = p_tolerance;
}

float Danmaku::get_tolerance() const {
    return tolerance;
}

void Danmaku::set_shot_sprite_count(int p_count) {
    ERR_FAIL_COND(p_count < 1);
    sprites.resize(p_count);
    _change_notify();
}

int Danmaku::get_shot_sprite_count() const {
    return sprites.size();
}

void Danmaku::set_shot_sprite(int p_index, const Ref<ShotSprite>& p_sprite) {
    ERR_FAIL_INDEX(p_index, sprites.size());
    sprites.write[p_index] = p_sprite;
}

Ref<ShotSprite> Danmaku::get_shot_sprite(int p_index) const {
    ERR_FAIL_INDEX_V(p_index, sprites.size(), Ref<ShotSprite>());
    return sprites[p_index];
}

void Danmaku::set_atlas(const Ref<Texture>& p_atlas) {
    atlas = p_atlas;
}

Ref<Texture> Danmaku::get_atlas() const {
    return atlas;
}

void Danmaku::_validate_property(PropertyInfo& property) const {
	if (property.name.begins_with("shot_sprite_")) {
		int index = property.name.get_slicec('_', 2).to_int() - 1;
		if (index >= sprites.size()) {
			property.usage = 0;
			return;
		}
	}
}

void Danmaku::_update_buffer() {
    PoolRealArray::Write write = buffer.write();
    real_t* buf = write.ptr();
    int visible = 0;

    for (int i = 0; i != patterns.size(); ++i) {
        visible += patterns[i]->fill_buffer(buf);
    }

    VS::get_singleton()->multimesh_set_as_bulk_array(multimesh, buffer);
    VS::get_singleton()->multimesh_set_visible_instances(multimesh, visible);
}

void Danmaku::_create_mesh() {
    Vector<Vector2> vertices;
    vertices.push_back(Vector2(-1, 1));
    vertices.push_back(Vector2(-1, -1));
    vertices.push_back(Vector2(1, -1));
    vertices.push_back(Vector2(1, 1));

    Vector<Vector2> uvs;
    uvs.push_back(Vector2(0, 1));
    uvs.push_back(Vector2(0, 0));
    uvs.push_back(Vector2(1, 0));
    uvs.push_back(Vector2(1, 1));

    Vector<int> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);

    Array array;
    array.resize(VS::ARRAY_MAX);
    array[VS::ARRAY_VERTEX] = vertices;
    array[VS::ARRAY_TEX_UV] = uvs;
    array[VS::ARRAY_INDEX] = indices;

    VS::get_singleton()->mesh_add_surface_from_arrays(mesh, VS::PRIMITIVE_TRIANGLES, array);
    VS::get_singleton()->multimesh_allocate(multimesh, 0, VS::MULTIMESH_TRANSFORM_2D, VS::MULTIMESH_COLOR_NONE, VS::MULTIMESH_CUSTOM_DATA_FLOAT);
    VS::get_singleton()->multimesh_set_mesh(multimesh, mesh);
}

void Danmaku::_create_material() {
    String code = "shader_type canvas_item;";

    code += "void vertex() {";
    code += "    UV *= INSTANCE_CUSTOM.xy;";
    code += "    UV += INSTANCE_CUSTOM.zw;";
    code += "}";

    VS::get_singleton()->shader_set_code(shader, code);
    VS::get_singleton()->material_set_shader(material, shader);
    VS::get_singleton()->canvas_item_set_material(get_canvas_item(), material);
}

void Danmaku::_destroy() {
    if (hitbox) {
        hitbox->remove_from_danmaku();
    }
    for (int i = 0; i != patterns.size(); ++i) {
        patterns[i]->remove_from_danmaku();
    }

    for (int i = 0; i != free_shots.size(); ++i) {
        memdelete(free_shots[i]);
    }
}

void Danmaku::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_update_buffer"), &Danmaku::_update_buffer);

    ClassDB::bind_method(D_METHOD("clear_circle"), &Danmaku::clear_circle);
    ClassDB::bind_method(D_METHOD("clear_rect"), &Danmaku::clear_rect);
    
    ClassDB::bind_method(D_METHOD("get_free_shot_count"), &Danmaku::get_free_shot_count);
    ClassDB::bind_method(D_METHOD("get_active_shot_count"), &Danmaku::get_active_shot_count);
    ClassDB::bind_method(D_METHOD("get_pattern_count"), &Danmaku::get_pattern_count);

    ClassDB::bind_method(D_METHOD("set_max_shots", "max_shots"), &Danmaku::set_max_shots);
    ClassDB::bind_method(D_METHOD("set_region", "region"), &Danmaku::set_region);
    ClassDB::bind_method(D_METHOD("set_tolerance", "tolerance"), &Danmaku::set_tolerance);
    ClassDB::bind_method(D_METHOD("set_atlas", "atlas"), &Danmaku::set_atlas);

    ClassDB::bind_method(D_METHOD("get_max_shots"), &Danmaku::get_max_shots);
    ClassDB::bind_method(D_METHOD("get_region"), &Danmaku::get_region);
    ClassDB::bind_method(D_METHOD("get_tolerance"), &Danmaku::get_tolerance);
    ClassDB::bind_method(D_METHOD("get_atlas"), &Danmaku::get_atlas);

    ClassDB::bind_method(D_METHOD("set_shot_sprite_count", "count"), &Danmaku::set_shot_sprite_count);
    ClassDB::bind_method(D_METHOD("get_shot_sprite_count"), &Danmaku::get_shot_sprite_count);
    ClassDB::bind_method(D_METHOD("set_shot_sprite", "index", "sprite"), &Danmaku::set_shot_sprite);
    ClassDB::bind_method(D_METHOD("get_shot_sprite", "index"), &Danmaku::get_shot_sprite);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_shots"), "set_max_shots", "get_max_shots");
    ADD_PROPERTY(PropertyInfo(Variant::RECT2, "region"), "set_region", "get_region");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "tolerance"), "set_tolerance", "get_tolerance");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "atlas", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_atlas", "get_atlas");

    ADD_GROUP("Sprites", "shot_");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "shot_sprites", PROPERTY_HINT_EXP_RANGE, "0," + itos(MAX_SHOT_SPRITES) + ",1"), "set_shot_sprite_count", "get_shot_sprite_count");
    for (int i = 0; i != MAX_SHOT_SPRITES; ++i) {
        ADD_PROPERTYI(PropertyInfo(Variant::OBJECT, "shot_sprite_" + itos(i + 1), PROPERTY_HINT_RESOURCE_TYPE, "ShotSprite"), "set_shot_sprite", "get_shot_sprite", i);
    }
}

Danmaku::Danmaku() {
    multimesh = VS::get_singleton()->multimesh_create();
    material = VS::get_singleton()->material_create();
    shader = VS::get_singleton()->shader_create();
    mesh = VS::get_singleton()->mesh_create();

    _create_material();
    _create_mesh();
    
    hitbox = NULL;
    region = Rect2(0, 0, 384, 448);
    tolerance = 64;
    max_shots = 0;
    set_shot_sprite_count(1);
    set_max_shots(2048);
}

Danmaku::~Danmaku() {
    _destroy();

    VS::get_singleton()->free(multimesh);
    VS::get_singleton()->free(mesh);
    VS::get_singleton()->free(material);
    VS::get_singleton()->free(shader);
}