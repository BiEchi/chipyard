// See LICENSE.txt for license details.
package modules

import chisel3._
import chisel3.iotesters.{Driver, TesterOptionsManager}
import utils.TutorialRunner

// run sbt "test:runMain modules.Launcher xxx"
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
    "chiselPra_qly" -> { (manager: TesterOptionsManager) => 
      Driver.execute(()=>new DeviceUnderTest(), manager) {
        (c) => new TesterSimple(c)
      }
    },
    "pipline_mul_4bit" -> { (manager: TesterOptionsManager) =>
      Driver.execute(() => new pipline_mul_4bit(), manager) {
        (c) => new pipline_mul_4bit_tester(c)
      }
    },
    "pipline_mul" -> { (manager: TesterOptionsManager) =>
      Driver.execute(() => new pipline_mul(8), manager) {
        (c) => new pipline_mul_tester(c)
      }
    },
    "pipline_div" -> { (manager: TesterOptionsManager) =>
      Driver.execute(() => new pipline_div(8), manager) {
        (c) => new pipline_div_test(c)
      }
    }
  )

  def main(args: Array[String]): Unit = {
    TutorialRunner("modules", tests, args)
  }
}
