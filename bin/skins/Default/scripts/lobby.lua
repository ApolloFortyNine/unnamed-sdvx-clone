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

-- Multiplayer rectanlge variables
multiplayer_lobby_rect = 300

-- Player slot vars
player_slot_starting_y = 105
player_slot_height = 32
player_slot_left_border = 20
player_slot_y_space = 8
num_players = 8

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
    gfx.Text("Multiplayer Lobby", 10, 35);
    gfx.BeginPath();
    gfx.FontSize(28);
    gfx.Text(string.format("%.1i", 18247126512642), 10, 60);
    gfx.BeginPath();

    -- Draw Player Slots
    for i = player_slot_starting_y, player_slot_starting_y + (num_players * (player_slot_y_space + player_slot_height)), 
    				player_slot_y_space + player_slot_height do
	    draw_player_slots(resx, resy, i);
    end

    -- Song Select
    draw_button("Song Select", resx - buttonWidth, (resy/2)+30, LobbyButtons.SongSelectMulti);
    gfx.BeginPath();

    -- Quit Button
    draw_button("Quit", resx - buttonWidth, resy - buttonHeight, LobbyButtons.Quit);
    gfx.BeginPath();
end;

mouse_pressed = function(button)
    if hovered then
        hovered()
    end
    return 0
end
