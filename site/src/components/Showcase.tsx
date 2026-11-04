import { useEffect, useRef, useState } from 'react'
import { gsap } from '../motion'

const steps = [
  {
    title: 'Load and edit',
    text: 'LoadFile reads any release, ASCII or binary. Edit::Add gives a new layer or line its handle and owner.',
  },
  {
    title: 'Save to R12',
    text: 'SaveFile writes the release asked for. What R12 cannot hold is drawn with what it has, and listed.',
  },
  {
    title: 'Audit',
    text: 'Check finds what AutoCAD would refuse or repair; Repair fixes it in place.',
  },
]

export function Showcase({ html }: { html: string }) {
  const code = useRef<HTMLDivElement>(null)
  const [active, setActive] = useState(0)
  useEffect(() => {
    const blocks = code.current?.querySelectorAll<HTMLElement>('[data-block]') ?? []
    for (const one of blocks) one.hidden = one.dataset.block !== String(active)
    const shown = code.current?.querySelector(`[data-block="${active}"]`)
    if (shown && window.matchMedia('(prefers-reduced-motion: no-preference)').matches) {
      gsap.fromTo(shown, { opacity: 0, y: 6 }, { opacity: 1, y: 0, duration: 0.35, ease: 'power2.out' })
    }
  }, [active])
  return (
    <div className="showcase-body">
      <ol className="steps" role="tablist" aria-label="Steps of the example">
        {steps.map((one, index) => (
          <li key={one.title} role="presentation">
            <button type="button" role="tab" aria-selected={active === index} onClick={() => setActive(index)}>
              <span className="step-index">{String(index + 1).padStart(2, '0')}</span>
              <span className="step-text">
                <strong>{one.title}</strong>
                <span>{one.text}</span>
              </span>
            </button>
          </li>
        ))}
      </ol>
      <div className="showcase-code" ref={code} dangerouslySetInnerHTML={{ __html: html }} />
    </div>
  )
}
