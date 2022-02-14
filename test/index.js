const { Chess } = require('chess.js')
const { exec } = require('child_process')
const kokopu = require('kokopu')

const { Position } = kokopu

//exec('../excalibur/excalibur', (error, stdout, stderr) => {
//    const lines = stdout.split('\n')
//    for (const line of lines) {
//        if (line === '')
//            return

//        const moves = line.split(' ')
//        moves.pop()
//        const chess = new Chess()
//        for (const move of moves) {
//            if (!chess.move(move, { sloppy: true })) {
//                console.log(moves)
//            }
//        }
        
//        const history = chess.history({ verbose: true })
//        const lastMove = history.pop()
//        if (lastMove.flags !== 'c') {
//            console.log(moves)
//        }
//        chess.clear()        
//    }
//})

exec('../excalibur', (error, stdout, stderr) => {
    console.log(error)
    const lines = stdout.split('\n')
    for (const line of lines) {
        if (line === '')
            return

        const moves = line.split(' ')
        moves.pop()
        console.log(moves)
        const position = new Position()
        for (const move of moves) {
            const descriptor = position.uci(move)
            if (!position.play(descriptor)) {
                console.log(moves)
            }
        }
        
        //const history = chess.history({ verbose: true })
        //const lastMove = history.pop()
        //if (lastMove.flags !== 'c') {
        //    console.log(moves)
        //}
        //chess.clear()        
    }
})

//const count_captures = depth => {
//    let captures = 0

//    const chess = new Chess()

//    const helper = depth => {
//        if (depth === 0)
//            return captures
        
//        const moves = chess.moves({ verbose: true })
//        //captures += moves.filter(move => move.flags === 'c').length
        
//        for (const move of moves) {
//            if (!chess.move(move)) {
//                console.log('bad!!')
//            }
//            helper(depth - 1)
//            chess.undo()
//        }

//        return captures
//    }

//    return helper(depth)
//}

//console.log(count_captures(4))