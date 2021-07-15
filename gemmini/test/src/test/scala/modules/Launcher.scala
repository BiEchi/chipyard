// See LICENSE.txt for license details.
package modules

import chisel3._
import chisel3.iotesters.{Driver, TesterOptionsManager}
import utils.TutorialRunner

object Launcher {
  val tests = Map(
    "shift_mul" -> { (manager: TesterOptionsManager) =>
      Driver.execute(() => new shift_mul(), manager) {
        (c) => new shift_mul_test(c)
      }
    },
    "shift_div" -> { (manager: TesterOptionsManager) =>
      Driver.execute(() => new shift_div(8), manager) {
        (c) => new shift_div_test(c)
      }
    },
    "pipline_div" -> { (manager: TesterOptionsManager) =>
      Driver.execute(() => new pipline_div(8), manager) {
        (c) => new pipline_div_test(c)
      }
    },
    "adder_tree" -> { (manager: TesterOptionsManager) =>
      Driver.execute(() => new adder_array(8,16), manager) {
        (c) => new adder_tree_test(c)
      }
    }  
  )

  def main(args: Array[String]): Unit = {
    TutorialRunner("modules", tests, args)
  }
}
