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
    }
  )

  def main(args: Array[String]): Unit = {
    TutorialRunner("modules", tests, args)
  }
}
