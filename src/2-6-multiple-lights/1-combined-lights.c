//------------------------------------------------------------------------------
//  Multiple Lights (1)
//------------------------------------------------------------------------------
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol/sokol_helper.h"
#include "sokol_log.h"
#include "hmm/HandmadeMath.h"
#include "1-combined-lights.glsl.h"
#define LOPGL_APP_IMPL
#include "../lopgl_app.h"

/* application state */
static struct {
    sg_pipeline pip_object;
    sg_bindings bind_object;
    sg_pipeline pip_light;
    sg_bindings bind_light;
    sg_pass_action pass_action;
    HMM_Vec3 cube_positions[10];
    HMM_Vec4 light_positions[4];
    uint8_t file_buffer[512 * 1024];
} state;

static void fail_callback() {
    state.pass_action = (sg_pass_action) {
        .colors[0] = { .load_action=SG_LOADACTION_CLEAR, .clear_value = { 1.0f, 0.0f, 0.0f, 1.0f } }
    };
}

static void init(void) {
    lopgl_setup();

    sg_alloc_image_smp(state.bind_object.fs, SLOT__diffuse_texture, SLOT_diffuse_texture_smp);
    sg_alloc_image_smp(state.bind_object.fs, SLOT__specular_texture, SLOT_specular_texture_smp);

    state.cube_positions[0] = HMM_V3( 0.0f,  0.0f,  0.0f);
    state.cube_positions[1] = HMM_V3( 2.0f,  5.0f, -15.0f);
    state.cube_positions[2] = HMM_V3(-1.5f, -2.2f, -2.5f);
    state.cube_positions[3] = HMM_V3(-3.8f, -2.0f, -12.3f);
    state.cube_positions[4] = HMM_V3( 2.4f, -0.4f, -3.5f);
    state.cube_positions[5] = HMM_V3(-1.7f,  3.0f, -7.5f);
    state.cube_positions[6] = HMM_V3( 1.3f, -2.0f, -2.5f);
    state.cube_positions[7] = HMM_V3( 1.5f,  2.0f, -2.5f);
    state.cube_positions[8] = HMM_V3( 1.5f,  0.2f, -1.5f);
    state.cube_positions[9] = HMM_V3(-1.3f,  1.0f, -1.5f);

    // positions of the point lights
    state.light_positions[0] = HMM_V4( 0.7f,  0.2f,  2.0f, 1.0f);
    state.light_positions[1] = HMM_V4( 2.3f, -3.3f, -4.0f, 1.0f);
    state.light_positions[2] = HMM_V4(-4.0f,  2.0f, -12.0f, 1.0f);
    state.light_positions[3] = HMM_V4( 0.0f,  0.0f, -3.0f, 1.0f);

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    sg_buffer cube_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(vertices),
        .data = SG_RANGE(vertices),
        .label = "cube-vertices"
    });
    
    state.bind_object.vertex_buffers[0] = cube_buffer;
    state.bind_light.vertex_buffers[0] = cube_buffer;

    /* create shader from code-generated sg_shader_desc */
    sg_shader phong_shd = sg_make_shader(phong_shader_desc(sg_query_backend()));

    /* create a pipeline object for object */
    state.pip_object = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = phong_shd,
        /* if the vertex layout doesn't have gaps, don't need to provide strides and offsets */
        .layout = {
            .attrs = {
                [ATTR_vs_a_pos].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_vs_a_normal].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_vs_a_tex_coords].format = SG_VERTEXFORMAT_FLOAT2
            }
        },
        .depth = {
            .compare =SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled =true,
        },
        .label = "object-pipeline"
    });

    /* create shader from code-generated sg_shader_desc */
    sg_shader light_cube_shd = sg_make_shader(light_cube_shader_desc(sg_query_backend()));

    /* create a pipeline object for light cube */
    state.pip_light = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = light_cube_shd,
        .layout = {
            .attrs = {
                [ATTR_vs_a_pos].format = SG_VERTEXFORMAT_FLOAT3
            },
            .buffers[0].stride = 32
        },
        .depth = {
            .compare =SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled =true,
        },
        .label = "light-cube-pipeline"
    });

    /* a pass action to clear framebuffer */
    state.pass_action = (sg_pass_action) {
        .colors[0] = { .load_action=SG_LOADACTION_CLEAR, .clear_value={0.1f, 0.1f, 0.1f, 1.0f} }
    };

    sg_image img_id_diffuse = state.bind_object.fs.images[SLOT__diffuse_texture];
    sg_image img_id_specular = state.bind_object.fs.images[SLOT__specular_texture];

    lopgl_load_image(&(lopgl_image_request_t){
            .path = "container2.png",
            .img_id = img_id_diffuse,
            .buffer_ptr = state.file_buffer,
            .buffer_size = sizeof(state.file_buffer),
            .fail_callback = fail_callback
    });

    lopgl_load_image(&(lopgl_image_request_t){
            .path = "container2_specular.png",
            .img_id = img_id_specular,
            .buffer_ptr = state.file_buffer,
            .buffer_size = sizeof(state.file_buffer),
            .fail_callback = fail_callback
    });
}

void frame(void) {
    lopgl_update();

    sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());

    sg_apply_pipeline(state.pip_object);
    sg_apply_bindings(&state.bind_object);

    fs_params_t fs_params = {
        .view_pos = lopgl_camera_position(),
        .material_shininess = 32.0f,
    };
    sg_apply_uniforms(SG_SHADERSTAGE_FS, SLOT_fs_params, &SG_RANGE(fs_params));
    
    fs_dir_light_t fs_dir_light = {
        .direction = HMM_V3(-0.2f, -1.0f, -0.3f),
        .ambient = HMM_V3(0.05f, 0.05f, 0.05f),
        .diffuse = HMM_V3(0.4f, 0.4f, 0.4f),
        .specular = HMM_V3(0.5f, 0.5f, 0.5f)
    };
    sg_apply_uniforms(SG_SHADERSTAGE_FS, SLOT_fs_dir_light, &SG_RANGE(fs_dir_light));

    fs_point_lights_t fs_point_lights = {
        .position[0]    = state.light_positions[0],
        .ambient[0]     = HMM_V4(0.05f, 0.05f, 0.05f, 0.0f),
        .diffuse[0]     = HMM_V4(0.8f, 0.8f, 0.8f, 0.0f),
        .specular[0]    = HMM_V4(1.0f, 1.0f, 1.0f, 0.0f),
        .attenuation[0] = HMM_V4(1.0f, 0.09f, 0.032f, 0.0f),
        .position[1]    = state.light_positions[1],
        .ambient[1]     = HMM_V4(0.05f, 0.05f, 0.05f, 0.0f),
        .diffuse[1]     = HMM_V4(0.8f, 0.8f, 0.8f, 0.0f),
        .specular[1]    = HMM_V4(1.0f, 1.0f, 1.0f, 0.0f),
        .attenuation[1] = HMM_V4(1.0f, 0.09f, 0.032f, 0.0f),
        .position[2]    = state.light_positions[2],
        .ambient[2]     = HMM_V4(0.05f, 0.05f, 0.05f, 0.0f),
        .diffuse[2]     = HMM_V4(0.8f, 0.8f, 0.8f, 0.0f),
        .specular[2]    = HMM_V4(1.0f, 1.0f, 1.0f, 0.0f),
        .attenuation[2] = HMM_V4(1.0f, 0.09f, 0.032f, 0.0f),
        .position[3]    = state.light_positions[3],
        .ambient[3]     = HMM_V4(0.05f, 0.05f, 0.05f, 0.0f),
        .diffuse[3]     = HMM_V4(0.8f, 0.8f, 0.8f, 0.0f),
        .specular[3]    = HMM_V4(1.0f, 1.0f, 1.0f, 0.0f),
        .attenuation[3] = HMM_V4(1.0f, 0.09f, 0.032f, 0.0f)
    };
    sg_apply_uniforms(SG_SHADERSTAGE_FS, SLOT_fs_point_lights, &SG_RANGE(fs_point_lights));
    
    fs_spot_light_t fs_spot_light = {
        .position = lopgl_camera_position(),
        .direction = lopgl_camera_direction(),
        .cut_off = HMM_COSF(HMM_ToRad(12.5f)),
        .outer_cut_off = HMM_COSF(HMM_ToRad(15.0f)),
        .attenuation = HMM_V3(1.0f, 0.09f, 0.032f),
        .ambient = HMM_V3(0.0f, 0.0f, 0.0f),
        .diffuse = HMM_V3(1.0f, 1.0f, 1.0f),
        .specular = HMM_V3(1.0f, 1.0f, 1.0f)
    };

    sg_apply_uniforms(SG_SHADERSTAGE_FS, SLOT_fs_spot_light, &SG_RANGE(fs_spot_light));

    HMM_Mat4 view = lopgl_view_matrix();
    HMM_Mat4 projection = HMM_Perspective_RH_NO(lopgl_fov(), (float)sapp_width() / (float)sapp_height(), 0.1f, 100.0f);

    vs_params_t vs_params = {
        .view = view,
        .projection = projection
    };

    for(size_t i = 0; i < 10; i++) {
        HMM_Mat4 model = HMM_Translate(state.cube_positions[i]);
        float angle = 20.0f * i; 
        model = HMM_MulM4(model, HMM_Rotate_RH(angle, HMM_V3(1.0f, 0.3f, 0.5f)));
        vs_params.model = model;
        sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));

        sg_draw(0, 36, 1);
    }

    sg_apply_pipeline(state.pip_light);
    sg_apply_bindings(&state.bind_light);

    HMM_Mat4 scale = HMM_Scale(HMM_V3(0.2f, 0.2f, 0.2f));

    for(size_t i = 0; i < 4; i++) {
        HMM_Vec3 pos = HMM_V3(state.light_positions[i].X, state.light_positions[i].Y, state.light_positions[i].Z);
        vs_params.model = HMM_Translate(pos);
        vs_params.model = HMM_MulM4(vs_params.model, scale);
        sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));

        sg_draw(0, 36, 1);
    }

    lopgl_render_help();

    sg_end_pass();
    sg_commit();
}

void event(const sapp_event* e) {
    lopgl_handle_input(e);
}

void cleanup(void) {
    lopgl_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .width = 800,
        .height = 600,
        .high_dpi = true,
        .window_title = "Combined Lights (LearnOpenGL)",
        //.logger.func = slog_func,
    };
}
