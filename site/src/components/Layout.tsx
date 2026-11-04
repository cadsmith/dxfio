import { useEffect, useState, type ReactNode } from 'react'
import { MenuIcon } from 'lucide-react'
import { Button } from '@/components/ui/button'
import { Sheet, SheetContent, SheetHeader, SheetTitle, SheetTrigger } from '@/components/ui/sheet'

export type Heading = { id: string; title: string; href: string }

type Props = { sidebar: (close: () => void) => ReactNode; outline: Heading[]; children: ReactNode }

const useActive = (headings: Heading[]) => {
  const [active, setActive] = useState<string | undefined>(headings[0]?.id)
  useEffect(() => {
    const update = () => {
      let current = headings[0]?.id
      for (const one of headings) {
        const element = document.getElementById(one.id)
        if (element && element.getBoundingClientRect().top <= 120) current = one.id
      }
      setActive(current)
    }
    update()
    window.addEventListener('scroll', update, { passive: true })
    return () => window.removeEventListener('scroll', update)
  }, [headings])
  return active
}

export function Layout({ sidebar, outline, children }: Props) {
  const active = useActive(outline)
  const [open, setOpen] = useState(false)
  return (
    <div className="docs">
      <aside className="docs-sidebar">{sidebar(() => undefined)}</aside>
      <div className="docs-local">
        <Sheet open={open} onOpenChange={setOpen}>
          <SheetTrigger render={<Button variant="ghost" />}>
            <MenuIcon data-icon="inline-start" />
            Menu
          </SheetTrigger>
          <SheetContent side="left" className="docs-sheet">
            <SheetHeader>
              <SheetTitle>dxfio</SheetTitle>
            </SheetHeader>
            <div className="docs-sheet-body">{sidebar(() => setOpen(false))}</div>
          </SheetContent>
        </Sheet>
      </div>
      <div className="docs-content">
        <main className="docs-main">{children}</main>
        {outline.length > 0 && (
          <aside className="docs-outline">
            <nav aria-label="On this page">
              <p className="outline-title">On this page</p>
              <ol>
                {outline.map((one) => (
                  <li key={one.id}>
                    <a href={one.href} aria-current={active === one.id ? 'location' : undefined}>
                      {one.title}
                    </a>
                  </li>
                ))}
              </ol>
            </nav>
          </aside>
        )}
      </div>
    </div>
  )
}

type Group = { title: string; links: { href: string; label: string; current: boolean }[] }

export function Sidebar({ groups, onPick, top }: { groups: Group[]; onPick: () => void; top?: ReactNode }) {
  return (
    <nav className="sidebar" aria-label="Documentation">
      {top}
      {groups.map((group) => (
        <section key={group.title} className="sidebar-group">
          <p className="sidebar-title">{group.title}</p>
          <ol>
            {group.links.map((one) => (
              <li key={one.href}>
                <a href={one.href} aria-current={one.current ? 'page' : undefined} onClick={onPick}>
                  {one.label}
                </a>
              </li>
            ))}
          </ol>
        </section>
      ))}
    </nav>
  )
}
