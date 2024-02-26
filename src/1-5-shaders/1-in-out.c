//------------------------------------------------------------------------------
//  1-5-1-in-out
//------------------------------------------------------------------------------
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "1-in-out.glsl.h"

/* application state */
static struct {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
} state;

static void init(void) {
    sg_setup(&(sg_desc){
        .context = sapp_sgcontext()
    });

    /* create shader from code-generated sg_shader_desc */
    sg_shader shd = sg_make_shader(simple_shader_desc(sg_query_backend()));

    /* a vertex buffer with 3 vertices */
    float vertices[] = {
        // positions
        -0.5f, -0.5f, 0.0f,     // bottom left
        0.5f, -0.5f, 0.0f,      // bottom right
        0.0f,  0.5f, 0.0f       // top
    };
    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(vertices),
        .data = SG_RANGE(vertices),
        .label = "triangle-vertices"
    });

    /* create a pipeline object (default render states are fine for triangle) */
    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        /* if the vertex layout doesn't have gaps, don't need to provide strides and offsets */
        .layout = {
            .attrs = {
                [ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3
            }
        },
        .label = "triangle-pipeline"
    });
    
    /* a pass action to clear framebuffer */
    state.pass_action = (sg_pass_action) {
        .colors[0] = { .load_action=SG_LOADACTION_CLEAR, .clear_value={0.2f, 0.3f, 0.3f, 1.0f} }
    };
}

void frame(void) {
    sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 3, 1);
    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    sg_shutdown();
}

void event(const sapp_event* e) {
    if (e->type == SAPP_EVENTTYPE_KEY_DOWN) {
        if (e->key_code == SAPP_KEYCODE_ESCAPE) {
            sapp_request_quit();
        }
    }
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
        .window_title = "In Out - LearnOpenGL",
    };
}
