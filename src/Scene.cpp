#include "Scene.h"
#include "objects/Room.h"
#include "objects/Bedroom.h"
#include "objects/Study.h"
#include "objects/Decorations.h"
#include "objects/Fixtures.h"

void drawScene(DrawContext& ctx, bool overview) {
    const glm::mat4 room(1);
    drawRoom(ctx,overview);
    drawBed(ctx,transform(room,{-1.75f,0,0.50f}));
    drawBedsideTable(ctx,transform(room,{-2.85f,0,-0.15f}));
    drawBedsideLamp(ctx,transform(room,{-2.85f,0.583f,-0.15f}));
    drawWardrobe(ctx,transform(room,{-2.15f,0,-2.36f}));
    drawTable(ctx,transform(room,{1.85f,0,-2.20f}));
    drawChair(ctx,transform(room,{1.85f,0,-1.28f},{1,1,1},-12));
    drawLaptop(ctx,transform(room,{1.60f,0.771f,-2.16f}));
    drawBookshelf(ctx,transform(room,{1.85f,1.46f,-2.65f}));
    const auto window=transform(room,{-0.05f,1.775f,-2.80f});
    drawWindow(ctx,window);
    drawCurtains(ctx,window);
    drawClock(ctx,transform(room,{2.45f,2.64f,-2.75f}));
    drawWallFrame(ctx,transform(room,{-3.16f,1.88f,0.10f},{1,1,1},90));
    drawRug(ctx,transform(room,{0.25f,0,0.85f}));
    drawDoor(ctx,transform(room,{3.23f,0,1.125f},{1,1,1},-90));
    // In cutaway mode ceiling-mounted fixtures are hidden with the ceiling.
    if(!overview) {
        drawFan(ctx,transform(room,{0,3.10f,0.1f}));
        drawCeilingLight(ctx,transform(room,{0,3.02f,-1.0f}));
    }
}
