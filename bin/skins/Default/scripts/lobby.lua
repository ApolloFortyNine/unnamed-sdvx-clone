-- Lobby GUI implementation
local mposx = 0;
local mposy = 0;
local hovered = nil;
local buttonWidth = 250;
local buttonHeight = 75;
local buttonBorder = 2;
local label = -1;
gfx.GradientColors(0,128,255,255,0,128,255,0)
local gradient = gfx.LinearGradient(0,0,0,1)

-- Song Cache
-- check_or_create_cache = function(song, loadJacket)
--     if not songCache[song.id] then songCache[song.id] = {} end

--     if not songCache[song.id]["title"] then
--         songCache[song.id]["title"] = gfx.CreateLabel(song.title, 40, 0)
--     end

--     if not songCache[song.id]["artist"] then
--         songCache[song.id]["artist"] = gfx.CreateLabel(song.artist, 25, 0)
--     end

--     if not songCache[song.id]["bpm"] then
--         songCache[song.id]["bpm"] = gfx.CreateLabel(string.format("BPM: %s",song.bpm), 20, 0)
--     end

--     if not songCache[song.id]["jacket"] and loadJacket then
--         songCache[song.id]["jacket"] = gfx.CreateImage(song.difficulties[1].jacketPath, 0)
--     end
-- end

-- Map Selection
map_id = -1

-- Multiplayer rectangle variables
multiplayer_lobby_rect = 300

-- Player slot vars
player_slot_starting_y = 105
player_slot_height = 32
player_slot_left_border = 20
player_slot_y_space = 8
num_players = 8
left_pad = 35

mouse_clipped = function(x,y,w,h)
    return mposx > x and mposy > y and mposx < x+w and mposy < y+h;
end;

draw_button = function(name, x, y, hoverindex)
    local rx = x - (buttonWidth / 2);
    local ty = y - (buttonHeight / 2);
    gfx.BeginPath();
    gfx.FillColor(0,128,255);
    if mouse_clipped(rx,ty, buttonWidth, buttonHeight) then
       hovered = hoverindex;
       gfx.FillColor(255,128,0);
    end
    gfx.Rect(rx - buttonBorder,
        ty - buttonBorder,
        buttonWidth + (buttonBorder * 2),
        buttonHeight + (buttonBorder * 2));
    gfx.Fill();
    gfx.BeginPath();
    gfx.FillColor(40,40,40);
    gfx.Rect(rx, ty, buttonWidth, buttonHeight);
    gfx.Fill();
    gfx.BeginPath();
    gfx.FillColor(255,255,255);
    gfx.TextAlign(gfx.TEXT_ALIGN_CENTER + gfx.TEXT_ALIGN_MIDDLE);
    gfx.FontSize(40);
    gfx.Text(name, x, y);
end;

draw_player_slots = function(resx, resy, ypos)
    -- Rectangle
    gfx.FillColor(240, 240, 245,50);
    gfx.Rect(player_slot_left_border,ypos,(6*resx)/10,player_slot_height);
    gfx.Fill();
    gfx.BeginPath();
end;

-- draw_song = function(song, x, y, w, h, selected)
--     -- check_or_create_cache(song)
--     gfx.BeginPath()
--     gfx.RoundedRectVarying(x,y,w,h,0,0,0,40)
--     gfx.FillColor(30,30,30)
--     gfx.StrokeColor(0,128,255)
--     gfx.StrokeWidth(1)
    
--     gfx.Fill()
--     gfx.Stroke()
--     gfx.FillColor(255,255,255)
--     gfx.TextAlign(gfx.TEXT_ALIGN_TOP + gfx.TEXT_ALIGN_LEFT)
--     -- gfx.DrawLabel(songCache[song.id]["title"], x+10, y + 5, w-10)
--     -- gfx.DrawLabel(songCache[song.id]["artist"], x+20, y + 50, w-10)
--     gfx.ForceRender()

-- end

draw_selected = function(song, x, y, w, h)
    -- check_or_create_cache(song)
    -- set up padding and margins
    local xPadding = math.floor(w/16)
    local yPadding =  math.floor(h/32)
    local xMargin = math.floor(w/16)
    local yMargin =  math.floor(h/32)
    local width = (w-(xMargin*2))
    local height = (h-(yMargin*2))
    local xpos = x+xMargin
    local ypos = y+yMargin
    if aspectRatio == "PortraitWidescreen" then
      xPadding = math.floor(w/64)
      yPadding =  math.floor(h/32)
      xMargin = math.floor(w/64)
      yMargin =  math.floor(h/32)
      width = (w-(xMargin*2))
      height = (h-(yMargin*2))
      xpos = x+xMargin
      ypos = y+yMargin
    end
    --Border
    -- local diff = song.difficulties[selectedDiff]
    -- effector = gfx.CreateLabel(diff.effector,20,0)
    gfx.BeginPath()
    gfx.RoundedRectVarying(xpos,ypos,width,height,yPadding,yPadding,yPadding,yPadding)
    gfx.FillColor(30,30,30)
    gfx.StrokeColor(0,128,255)
    gfx.StrokeWidth(1)
    gfx.Fill()
    gfx.Stroke()
    -- jacket should take up 1/3 of height, always be square, and be centered
    local imageSize = math.floor(height/3)
    local imageXPos = ((width/2) - (imageSize/2)) + x+xMargin
    if aspectRatio == "PortraitWidescreen" then
      --Unless its portrait widesreen..
      imageSize = math.floor((height/3)*2)
      imageXPos = x+xMargin+xPadding
    end

    -- local temp = gfx.LoadImageJob(diff.jacketPath, jacketFallback, 200,200)
    -- if not songCache[song.id][selectedDiff] or songCache[song.id][selectedDiff] ==  jacketFallback then
    --     songCache[song.id][selectedDiff] = gfx.LoadImageJob(diff.jacketPath, jacketFallback, 200,200)
    -- end

    -- if songCache[song.id][selectedDiff] then
        -- gfx.BeginPath()
        -- gfx.ImageRect(imageXPos, y+yMargin+yPadding, imageSize, imageSize, songCache[song.id][selectedDiff], 1, 0)
    -- end
    -- difficulty should take up 1/6 of height, full width, and be centered
    -- if aspectRatio == "PortraitWidescreen" then
      --difficulty wheel should be right below the jacketImage, and the same width as
      --the jacketImage
      -- draw_diffs(song.difficulties,xpos+xPadding,(ypos+yPadding+imageSize),imageSize,math.floor((height/3)*1)-yPadding)
    -- else
      -- difficulty should take up 1/6 of height, full width, and be centered
      -- draw_diffs(song.difficulties,(w/2)-(imageSize/2),(ypos+yPadding+imageSize),imageSize,math.floor(height/6))
    -- end
    -- effector / bpm should take up 1/3 of height, full width
    -- if aspectRatio == "PortraitWidescreen" then
    --   gfx.FontSize(40)
    --   gfx.TextAlign(gfx.TEXT_ALIGN_TOP + gfx.TEXT_ALIGN_LEFT)
    --   gfx.DrawLabel(songCache[song.id]["title"], xpos+xPadding+imageSize, y+yMargin+yPadding, width-imageSize-20)
    --   gfx.FontSize(30)
    --   gfx.DrawLabel(songCache[song.id]["artist"], xpos+xPadding+imageSize+3, y+yMargin+yPadding + 45, width-imageSize-20)
    --   gfx.FontSize(20)
    --   gfx.DrawLabel(songCache[song.id]["bpm"], xpos+xPadding+imageSize+3, y+yMargin+yPadding + 85, width-imageSize-20)
    --   gfx.FastText("Effector:", xpos+xPadding+imageSize+3, y+yMargin+yPadding + 115)
    --   gfx.DrawLabel(effector, xpos+xPadding+imageSize+80, y+yMargin+yPadding + 115, width-imageSize-20)
    -- else
    --   gfx.FontSize(40)
    --   gfx.TextAlign(gfx.TEXT_ALIGN_TOP + gfx.TEXT_ALIGN_LEFT)
    --   gfx.DrawLabel(songCache[song.id]["title"], xpos+10, (height/10)*6, width-20)
    --   gfx.FontSize(30)
    --   gfx.DrawLabel(songCache[song.id]["artist"], xpos+10, (height/10)*6 + 45, width-20)
    --   gfx.FillColor(255,255,255)
    --   gfx.FontSize(20)
    --   gfx.DrawLabel(songCache[song.id]["bpm"], xpos+10, (height/10)*6 + 85)
    --   gfx.FastText("Effector:",xpos+10, (height/10)*6 + 115)
    --   gfx.DrawLabel(effector, xpos+85, (height/10)*6 + 115, width-95)
    -- end
    -- if aspectRatio == "PortraitWidescreen" then
    --   draw_scores(diff, xpos+xPadding+imageSize+3,  (height/3)*2, width-imageSize-20, (height/3)-yPadding)
    -- else
    --   draw_scores(diff, xpos, (height/6)*5, width, (height/6))
    -- end
    gfx.ForceRender()
end

render = function(deltaTime)
    resx,resy = game.GetResolution();
    mposx,mposy = game.GetMousePos();
    gfx.Scale(resx, resy / 3);

    -- Background
    gfx.Rect(0,0,1,1);
    gfx.FillPaint(gradient);
    gfx.Fill();
    gfx.ResetTransform();
    gfx.BeginPath();

    -- quitButtonY = 5 * resy / 6;
    hovered = nil;
    gfx.LoadSkinFont("segoeui.ttf");

    -- Multiplayer Lobby Rectangle
    gfx.Translate(5,5) --upper left margin
    gfx.FillColor(30,30,47,200);
    gfx.Rect(0,0,multiplayer_lobby_rect,70);
    gfx.Fill();

    -- Multiplayer Lobby Rectangle Text
    gfx.FillColor(255,255,255);
    gfx.FontSize(42);
    gfx.Text("Multiplayer Lobby", 10, left_pad);
    gfx.BeginPath();
    gfx.FontSize(28);
    gfx.Text(string.format("%.1i", 18247126512642), 10, 60);
    gfx.BeginPath();

    -- Draw Player Slots
    for i = player_slot_starting_y, player_slot_starting_y + (num_players * (player_slot_y_space + player_slot_height)), 
                    player_slot_y_space + player_slot_height do
        draw_player_slots(resx, resy, i);
    end

    -- Draw song selected
    local song = map_id
    -- local xpos = x + offsetX + ((selectedIndex - i + ioffset) ^ 2) * 3
    -- local offsetY = (selectedIndex - i + ioffset) * ( height - (wheelSize/2*((selectedIndex-i + ioffset)*aspectFloat)))
    -- local ypos = y+((h/2 - height/2) - offsetY)
    local xpos = resx - 1.5*buttonWidth
    local ypos = player_slot_starting_y
    local width = 250
    local height = 200
    draw_selected(song, xpos, ypos, width, height);

    -- Song Start
    draw_button("Start Game", (buttonWidth / 2) + left_pad, resy - buttonHeight, LobbyButtons.StartGame);
    gfx.BeginPath();

    -- Song Select
    draw_button("Song Select", resx - buttonWidth, (resy/2)+30, LobbyButtons.SongSelectMulti);
    gfx.BeginPath();

    -- Quit Button
    draw_button("Quit", resx - buttonWidth, resy - buttonHeight, LobbyButtons.Quit);
    gfx.BeginPath();
end;

set_map_select = function(new_map_id)
    if new_map_id ~= map_id then
        game.PlaySample("menu_click")
        map_id = new_map_id
    end
end;

mouse_pressed = function(button)
    if hovered then
        hovered()
    end
    return 0
end
