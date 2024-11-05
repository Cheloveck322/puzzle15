#include <array>
#include <iostream>
#include <numeric>
#include <cassert>
#include "Random.h"

// Increase amount of new lines if your board isn't
// at the very bottom of the console
constexpr int g_consoleLines{ 25 };

class Direction
{
public:
    enum Type
    {
        up, 
        down,
        left,
        right,
        max_direction
    };

    Direction(Type type)
        : m_type { type }
    {
    }
    Type getType() const
    {
        return m_type;
    }

    Direction operator-() const 
    {
        switch (m_type)
        {
        case up:    return Direction{ down };
        case down:  return Direction{ up };
        case left:  return Direction{ right };
        case right: return Direction{ left };
        };

        assert(0 && "Unsupported direction was passed!");
        return Direction{ up };
    }

    friend std::ostream& operator<<(std::ostream& out, Direction dir)
    {
        switch (dir.m_type)
        {
        case Direction::up:    return (out << "up");
        case Direction::down:  return (out << "down");
        case Direction::left:  return (out << "left");
        case Direction::right: return (out << "right");
        default:               return (out << "unknown direction");
        }
    }

    static Direction randomDirection()
    {
        Type random { static_cast<Type>(Random::get(0, Type::max_direction - 1)) };
        return Direction{ random };
    } 

private:
    Type m_type {};
};

struct Point
{
    int x {};
    int y {};

    friend bool operator==(const Point& a, const Point& b)
    {
        return (a.x == b.x && a.y == b.y);
    }

    friend bool operator!=(const Point& a, const Point& b)
    {
        return !(a == b);
    }

    Point getAdjacentPoint(Direction dir) const
    {
        switch (dir.getType())
        {
        case Direction::up:     return Point{ x,      y - 1 };
        case Direction::down:   return Point{ x,      y + 1 };
        case Direction::left:   return Point{ x - 1,  y };
        case Direction::right:  return Point{ x + 1,  y };
        }

        return *this;
    }
};

namespace UserInput
{
    void ignoreLine()
    {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    bool isValidCommand(char c)
    {
        return c == 'w'
            || c == 'a'
            || c == 's'
            || c == 'd'
            || c == 'q';
    }

    char getCharacter()
    {
        char operation{};
        std::cin >> operation;
        ignoreLine();
        return operation;
    }

    char getCommandFromUser()
    {
        char ch{};
        while (!isValidCommand(ch))
        {
            ch = getCharacter();
        }

        return ch;
    }

    Direction charToDirection(char ch)
    {
        switch (ch)
        {
        case 'w':   return Direction{ Direction::up };
        case 's':   return Direction{ Direction::down };
        case 'a':   return Direction{ Direction::left };
        case 'd':   return Direction{ Direction::right };
        };
        
        assert(0 && "Invalid input was entered");
        return Direction{ Direction::up };
    }
} 

class Tile
{
public:
    Tile() = default;
    explicit Tile(int number)
        :m_number { number }
    {
    }

    bool isEmpty() const 
    {
        return m_number == 0;
    }

    int getNum() const 
    {
        return m_number;
    }

    friend std::ostream& operator<<(std::ostream& out, const Tile& tile)
    {
        if (tile.m_number > 9)
            out << " " << tile.m_number << " ";
        else if (tile.m_number > 0)
            out << "  " << tile.m_number << " ";
        else if (tile.m_number == 0)
            out << "    ";
        return out;
    }

private:    
    int m_number{};
};

class Board
{
public: 

    Board() = default;
    
    static void printEmptyLines(int count)
    {
        for (int i = 0; i < count; ++i)
            std::cout << '\n';
    }

    friend std::ostream& operator<<(std::ostream& out, Board board)
    {
        printEmptyLines(g_consoleLines);

        for (int y{}; y < m_sizeOfBoard; ++y)
        {
            for (int x{}; x < m_sizeOfBoard; ++x)
            {
                out << board.m_tiles[y][x];
            }
            out << '\n';
        }

        return out;
    }

    Point getEmptyTilePos() const
    {
        for (int y = 0; y < m_sizeOfBoard; ++y)
            for (int x = 0; x < m_sizeOfBoard; ++x)
                if (m_tiles[y][x].isEmpty())
                    return { x,y };

        assert(0 && "There is no empty tile in the board!!!");
        return { -1,-1 };
    }

    static bool isValidTilePos(Point pt)
    {
        return (pt.x >= 0 && pt.x < m_sizeOfBoard)
            && (pt.y >= 0 && pt.y < m_sizeOfBoard);
    }

    void swapTiles(Point pt1, Point pt2)
    {
        std::swap(m_tiles[pt1.y][pt1.x], m_tiles[pt2.y][pt2.x]);
    }

    friend bool operator==(const Board& b1, const Board& b2)
    {
        for (int y {}; y < m_sizeOfBoard; ++y)
        {
            for (int x {}; x < m_sizeOfBoard; ++x)
            {
                if (b1.m_tiles[y][x].getNum() != b2.m_tiles[y][x].getNum())
                    return false;
            }
        }
        return true;
    }

    bool moveTile(Direction dir)
    {
        Point emptyTile { getEmptyTilePos() };
        Point adj { emptyTile.getAdjacentPoint(-dir) };

        if (!isValidTilePos(adj))
            return false;
        
        swapTiles(adj, emptyTile);
        return true;
    }

    bool playerWon() const
    {
        static Board solvedBoard{};
        return (*this == solvedBoard);
    }

    void randomize()
    {
        for (int count {}; count < 1000; ++count)
        {
            bool success = moveTile(Direction::randomDirection());
            if (!success)
                --count;
        }
    }

private:
    static const int m_sizeOfBoard = 4;
    Tile m_tiles[m_sizeOfBoard][m_sizeOfBoard]{
        Tile { 1 }, Tile { 2 }, Tile { 3 }, Tile { 4 },
        Tile { 5 }, Tile { 6 }, Tile { 7 }, Tile { 8 },
        Tile { 9 }, Tile { 10 }, Tile { 11 }, Tile { 12 },
        Tile { 13 }, Tile { 14 }, Tile { 15 }, Tile { 0 } };
};

int main()
{
    Board board{};
    board.randomize();
    std::cout << board;

    while (!board.playerWon())
    {  
        char ch { UserInput::getCommandFromUser() };

        if ( ch == 'q' )
        {
            std::cout << "\n\nBye\n\n";
            return 0;
        }

        Direction dir { UserInput::charToDirection(ch) };

        bool userMoved { board.moveTile(dir) };
        if (userMoved)
            std::cout << board;

    }
    
    std::cout << "\n\nYou won!!!\n\n";
    return 0;
}