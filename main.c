#include "raylib.h"

typedef enum GameScreen { MENU, GAMEPLAY } GameScreen;
typedef enum CharState { IDLE, WALK } CharState;

// quantidade de paredes invisíveis
#define NUM_PAREDES 4

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Nosso RPG");

    Texture2D mapa = LoadTexture("snowdin_mapa.png");
    
    // carregando os sprites
    Texture2D texIdle = LoadTexture("Idle.png");
    Texture2D texWalk = LoadTexture("Walk.png");

    Vector2 playerPos = { (float)screenWidth / 2, (float)screenHeight / 2 };
    float playerSpeed = 4.0f;
    int facing = 1; // 1 = direita, -1 = esquerda

    // variáveis de animação
    CharState estadoAtual = IDLE;
    int currentFrame = 0;
    int framesCounter = 0;
    int framesSpeed = 10; 
    
    // tamanho da personagem
    float novoTamX = 120.0f; 
    float novoTamY = 120.0f;

    // configurações da camera
    Camera2D camera = { 0 };
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    GameScreen telaAtual = MENU;

    // definição das paredes invisiveis (x, y, largura, altura)
    // ATENÇÃO: vc precisará alterar os números testando o jogo para encaixar no mapa
    Rectangle paredes[NUM_PAREDES] = {
        { 0.0f, 0.0f, 1500.0f, 150.0f },    // parede do topo
        { 0.0f, 150.0f, 200.0f, 800.0f },    // parede esquerda
        { 800.0f, 150.0f, 300.0f, 800.0f },  // parede direita
        { 0.0f, 550.0f, 3000.0f, 300.0f }   // parede de baixo
    };

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // lógica
        if (telaAtual == MENU)
        {
            if (IsKeyPressed(KEY_ENTER)) telaAtual = GAMEPLAY;
        }
        else if (telaAtual == GAMEPLAY)
        {
            CharState estadoAnterior = estadoAtual;
            estadoAtual = IDLE;
            bool isMoving = false;

            // sala posição antes de mover
            Vector2 posAnterior = playerPos;

            // movimento e Direção
            if (IsKeyDown(KEY_RIGHT)) { playerPos.x += playerSpeed; facing = 1; isMoving = true; }
            if (IsKeyDown(KEY_LEFT))  { playerPos.x -= playerSpeed; facing = -1; isMoving = true; }
            if (IsKeyDown(KEY_DOWN))  { playerPos.y += playerSpeed; isMoving = true; }
            if (IsKeyDown(KEY_UP))    { playerPos.y -= playerSpeed; isMoving = true; }

            // colisão
            // Cria uma "caixa" menor em volta do corpo da personagem para a colisão ficar realista
            Rectangle playerHitbox = { 
                playerPos.x - (novoTamX / 4), 
                playerPos.y - (novoTamY / 4), 
                novoTamX / 2, 
                novoTamY / 2 
            };

            for (int i = 0; i < NUM_PAREDES; i++)
            {
                if (CheckCollisionRecs(playerHitbox, paredes[i]))
                {
                    playerPos = posAnterior; // se bater, cancela o movimento e volta um passo
                    break;
                }
            }

            if (isMoving) estadoAtual = WALK;
            if (estadoAtual != estadoAnterior) currentFrame = 0;

            int maxFrames = (estadoAtual == WALK) ? 12 : 9; 

            framesCounter++;
            if (framesCounter >= (60 / framesSpeed))
            {
                framesCounter = 0;
                currentFrame++;
                if (currentFrame >= maxFrames) currentFrame = 0;
            }

            camera.target = playerPos;
        }

        // --- renderização  ---
        BeginDrawing();
            ClearBackground(BLACK);

            if (telaAtual == MENU)
            {
                DrawText("my game!", screenWidth/2 - MeasureText("my game!", 60)/2, screenHeight/2 - 60, 60, RAYWHITE);
                DrawText("Aperte ENTER para Start", screenWidth/2 - MeasureText("Aperte ENTER para Start", 20)/2, screenHeight/2 + 20, 20, GRAY);
            }
            else if (telaAtual == GAMEPLAY)
            {
                BeginMode2D(camera);
                    DrawTexture(mapa, 0, 0, WHITE);

                    // 5. DESENHA AS PAREDES EM VERMELHO PARA DEPURAÇÃO
                    // Quando terminar de ajustar os valores (x, y, largura, altura), você pode apagar este loop!
           //         for (int i = 0; i < NUM_PAREDES; i++)
           //         {
           //             DrawRectangleRec(paredes[i], Fade(RED, 0.4f));
           //         }

                    Texture2D texAtual = (estadoAtual == WALK) ? texWalk : texIdle;
                    int maxFrames = (estadoAtual == WALK) ? 12 : 9; 

                    float frameWidth = (float)texAtual.width / maxFrames; 
                    float frameHeight = (float)texAtual.height;

                    Rectangle sourceRec = { currentFrame * frameWidth, 0.0f, frameWidth * facing, frameHeight };
                   
                    Rectangle destRec = { playerPos.x, playerPos.y, novoTamX, novoTamY };
                    Vector2 origin = { novoTamX / 2, novoTamY / 2 };

                    DrawTexturePro(texAtual, sourceRec, destRec, origin, 0.0f, WHITE);

                EndMode2D();
            }
        EndDrawing();
    }

    UnloadTexture(mapa);
    UnloadTexture(texIdle);
    UnloadTexture(texWalk);
    CloseWindow();

    return 0;
}