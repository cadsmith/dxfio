import { useEffect, useState, useTransition } from 'react'

const base = import.meta.env.BASE_URL

// The pieces of a path below the site's base, such as ['tutorials', 'layers'].
export const parse = (path: string) => path.slice(base.length).split('/').filter(Boolean).map(decodeURIComponent)

export const link = (...parts: string[]) => (parts.length > 0 ? `${base}${parts.join('/')}/` : base)

// A header's page; the one that includes them all is the reference's own.
export const reference = (path: string) =>
  path === 'dxfio.hpp' ? link('reference') : link('reference', ...path.replace(/\.hpp$/, '').split('/'))

export const navigate = (href: string) => {
  const url = new URL(href, window.location.href)
  if (url.pathname === window.location.pathname && url.hash) {
    window.location.hash = url.hash
    return
  }
  window.history.pushState(null, '', url)
  if (!url.hash) window.scrollTo({ top: 0 })
  window.dispatchEvent(new PopStateEvent('popstate'))
}

// Scrolls to the section the address names, once the page holding it is drawn.
export const reach = () => {
  const id = decodeURIComponent(window.location.hash.slice(1))
  if (id) document.getElementById(id)?.scrollIntoView({ block: 'start' })
}

export const useTitle = (title: string) => {
  useEffect(() => {
    document.title = title
  }, [title])
}

const followed = (event: MouseEvent) => {
  if (event.defaultPrevented || event.button !== 0 || event.metaKey || event.ctrlKey || event.shiftKey || event.altKey) return
  const anchor = (event.target as Element | null)?.closest('a')
  if (!anchor || anchor.target || anchor.hasAttribute('download')) return
  const url = new URL(anchor.href)
  if (url.origin !== window.location.origin || !url.pathname.startsWith(base)) return
  if (url.pathname === window.location.pathname && url.hash) return
  event.preventDefault()
  navigate(url.href)
}

// The route, and whether the next one is still loading.
export const useRoute = (at: string[]) => {
  const [route, setRoute] = useState(at)
  const [pending, start] = useTransition()
  useEffect(() => {
    // A transition: the page shown stays until the next one has loaded its code and text.
    const changed = () => { start(() => { setRoute(parse(window.location.pathname)) }) }
    window.addEventListener('popstate', changed)
    document.addEventListener('click', followed)
    return () => {
      window.removeEventListener('popstate', changed)
      document.removeEventListener('click', followed)
    }
  }, [start])
  return { route, pending }
}
